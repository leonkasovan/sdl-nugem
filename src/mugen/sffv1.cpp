/*
 * Copyright (c) 2016 Victor Nivet
 *
 * This file is part of Nugem.
 *
 * Nugem is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 * Nugem is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with Nugem.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "sffv1.hpp"

#include "../character.hpp"

#include <ios>
#include <fstream>
#include <iostream>
#include <array>

namespace Nugem {
namespace Mugen {

Sffv1::Sffv1(const char * filename, const char * paletteFile): m_filename(filename), m_paletteFile(paletteFile)
{
    m_currentSprite = 0;
    m_currentPalette = 0;
    m_sffv1Container.clear();
    m_palettes.clear();
    loadSffFile();
    loadSharedPalettes();
}

Sffv1::~Sffv1()
{
    for (int i = 0; i < m_sffv1Container.size(); i++) {
        delete [] m_sffv1Container[i].data;
    }
}

void Sffv1::loadSffFile()
{
    uint8_t * readbuf[READBUF_SIZE];
    std::ifstream charfile(m_filename.c_str());
    // First 512 bytes: header
    // Signature at the start of the file: 'ElecbyteSpr\0'
    charfile.read((char *) readbuf, 12);
    if (strcmp((char *) readbuf, "ElecbyteSpr")) {
        throw std::runtime_error(std::string("Invalid sprite file: ") + m_filename);
    }
    // Version bytes
    std::array<uint8_t, 4> version = extract_version(charfile);
    // if the version is too high, throw it
    if (version[3] > 1)
        throw version;
    // Number of groups
    m_ngroups = read_uint32(charfile);
    m_nimages = read_uint32(charfile);
    uint32_t nextSubfileOffset = read_uint32(charfile);
    uint32_t subheaderSize = read_uint32(charfile);
    uint8_t sharedPaletteByte;
    charfile >> sharedPaletteByte;
    m_sharedPalette = (sharedPaletteByte != 0);
    // Reading the subfiles
    while (charfile.good() && ((int) nextSubfileOffset) > 0 && m_sffv1Container.size() < m_nimages) {
        Sffv1Sprite sprite;
        charfile.seekg(nextSubfileOffset, std::ios::beg);
        nextSubfileOffset = read_uint32(charfile);
        sprite.dataSize = read_uint32(charfile);
        sprite.axisX = read_uint16(charfile);
        sprite.axisY = read_uint16(charfile);
        sprite.group = read_uint16(charfile);
        sprite.groupimage = read_uint16(charfile);
        sprite.linkedindex = read_uint16(charfile);
        uint8_t usesSharedPaletteByte;
        charfile >> usesSharedPaletteByte;
        sprite.usesSharedPalette = (usesSharedPaletteByte != 0);
        charfile.seekg(13, std::ios::cur); // skipping the next 12 bytes, from byte 19 to byte 31
        // According to formats.txt:
        // "PCX graphic data. If palette data is available, it is the last 768 bytes."
        sprite.data = new uint8_t[sprite.dataSize];
        charfile.read((char *) sprite.data, sprite.dataSize);
        // Add the sprites' index to its group and image number
        m_groups[sprite.group].i[sprite.groupimage] = m_sffv1Container.size();
        m_sffv1Container.emplace_back(std::move(sprite));
    }
    charfile.close();
}

void Sffv1::loadSharedPalettes()
{
	if (m_paletteFile.empty())
		return;
    bool continueLoop = true;
    // We get the values of pal1, pal2, pal3, ... etc in order until pal12
    for (int i = 1; continueLoop && i <= 12; i++) {
        std::string keyname = "pal";
        keyname += std::to_string(i);
        try {
            continueLoop = readActPalette(m_paletteFile.c_str());
        }
        catch
            (std::out_of_range) { // thrown by character.getdef() if the method at doesn't find the key
            break;
        }
    }
}

Sffv1Palette Sffv1::getPaletteForSprite(size_t spritenumber)
{
    Sffv1Palette s;
    long paletteSpriteNumber = spritenumber;
    size_t iterationNumber = m_sprites.size();
    if (m_sharedPalette && m_sffv1Container[paletteSpriteNumber].usesSharedPalette)
        return m_palettes[m_currentPalette];
    while (m_sffv1Container[paletteSpriteNumber].usesSharedPalette && iterationNumber > 0) {
        iterationNumber--;
        paletteSpriteNumber--;
        if (paletteSpriteNumber < 0)
            paletteSpriteNumber += m_sffv1Container.size();
    }
    Sffv1Sprite & paletteSprite = m_sffv1Container[paletteSpriteNumber];
    if (paletteSprite.dataSize > 768 && paletteSprite.data[paletteSprite.dataSize - 768 - 1] == 0x0C) {
        uint8_t * paletteData = paletteSprite.data + (paletteSprite.dataSize - 768);
        for (int i = 0; i < PALETTE_NCOLORS; i++) {
            s.colors[i] = (Sffv1Color) {
                *(paletteData + 3 * i), *(paletteData + 3 * i + 1), *(paletteData + 3 * i + 2)
            };
        }
        return s;
    }
    return m_palettes[m_currentPalette];
}

SDL_Surface * Sffv1::renderToSurface()
{
    Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
    size_t displayedSpriteNumber = m_currentSprite;
    if (m_sffv1Container[m_currentSprite].linkedindex && !m_sffv1Container[m_currentSprite].dataSize)
        displayedSpriteNumber = m_sffv1Container[m_currentSprite].linkedindex;
    Sffv1Sprite & displayedSprite = m_sffv1Container[displayedSpriteNumber];

// 	if the sprite is supposed to use the same palette as the previous sprite, then we get this palette
    SDL_Surface * surface = nullptr;

    // decoding the PCX
    uint16_t Xmin = *( (uint16_t * ) displayedSprite.data + 2);
    uint16_t Ymin = *( (uint16_t * ) displayedSprite.data + 3);
    uint16_t Xmax = *( (uint16_t * ) displayedSprite.data + 4);
    uint16_t Ymax = *( (uint16_t * ) displayedSprite.data + 5);
    uint8_t nplanes = *( (uint8_t * ) displayedSprite.data + 65);
    uint16_t bytesPerLine = *( (uint16_t * ) displayedSprite.data + 33);
    uint16_t width = Xmax - Xmin + 1;
    uint16_t height = Ymax - Ymin + 1;
    uint32_t totalBytesPerLine = nplanes * bytesPerLine;
    surface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);
    SDL_LockSurface(surface);
    // find the right palette
    Sffv1Palette palette = getPaletteForSprite(m_currentSprite);
    uint8_t * dataStart = displayedSprite.data + 128;
    uint32_t * pixels = (uint32_t * ) surface->pixels;
    size_t i_pixel, i_byte;
    for (i_pixel = 0, i_byte = 0; i_pixel < width * height && i_byte < (displayedSprite.dataSize - 128); i_byte++) {
        uint16_t runLength;
        SDL_Color sdlcolor;
        uint8_t colorIndex;
        if ((dataStart[i_byte] & 0xC0) == 0xC0) { // RLE byte
            runLength = dataStart[i_byte] & 0x3F;
            i_byte++;
            colorIndex = dataStart[i_byte];
        }
        else { // simple pixel byte
            runLength = 1;
            colorIndex = dataStart[i_byte];
        }
        if (colorIndex) { // if not null
            Sffv1Color & color = palette.colors[dataStart[i_byte]];
            sdlcolor = (SDL_Color) {
                color.red, color.green, color.blue, 0xFF
            };
        }
        else // apply transparency if the color value is zero
            sdlcolor = (SDL_Color) {
            0, 0, 0, 0
        };
        for (int runCount = runLength; runCount > 0; runCount--, i_pixel++)
            *(pixels + i_pixel) = SDL_MapRGBA(surface->format, sdlcolor.r, sdlcolor.g, sdlcolor.b, sdlcolor.a);
    }
    SDL_UnlockSurface(surface);

    return surface;
}

bool Sffv1::readActPalette(const char * filepath)
{
    Sffv1Palette palette;
    std::ifstream actfile;
    // reading a .act file: a Photoshop 8-bit palette
    try {
        actfile.open(filepath);
        if (actfile.fail())
            return false;
        // for some reason the colors are in reverse order
        // it didn't appear to be in the official specification though
        for (int i_palette = 0; i_palette < PALETTE_NCOLORS && actfile.good(); i_palette++) {
            palette.colors[PALETTE_NCOLORS - 1 - i_palette].red = actfile.get();
            palette.colors[PALETTE_NCOLORS - 1 - i_palette].green = actfile.get();
            palette.colors[PALETTE_NCOLORS - 1 - i_palette].blue = actfile.get();
        }
        actfile.close();
    }
    catch
        (std::ios_base::failure()) {
        if (actfile.is_open())
            actfile.close();
        return false;
    }
    m_palettes.push_back(palette);
    return true;
}

void Sffv1::load()
{
    m_sprites.clear();
    for (m_currentPalette = 0; m_currentPalette < m_palettes.size(); m_currentPalette++) {
        std::unordered_map<Spriteref, Sprite> currentPaletteSprites;
        for (m_currentSprite = 0; m_currentSprite < m_sffv1Container.size(); m_currentSprite++) {
            Sffv1Sprite & sprite = m_sffv1Container[m_currentSprite];
            Spriteref ref(sprite.group, sprite.groupimage);
            currentPaletteSprites.insert(std::pair<Spriteref, Sprite>(ref, Sprite(ref, renderToSurface(), m_currentPalette)));
        }
        m_sprites.push_back(currentPaletteSprites);
    }
}

void Sffv1::load(std::vector<Spriteref>::iterator first, std::vector<Spriteref>::iterator last)
{
    m_sprites.clear();
    for (m_currentPalette = 0; m_currentPalette < m_palettes.size(); m_currentPalette++) {
        m_sprites.push_back(std::unordered_map<Spriteref, Sprite>());
    }

    for (; first != last; first++) {
        for (m_currentPalette = 0; m_currentPalette < m_palettes.size(); m_currentPalette++) {
            Spriteref & ref = *first;
            m_currentSprite = m_groups[ref.group].i[ref.image];
            m_sprites[m_currentPalette].insert(std::pair<Spriteref, Sprite>(ref, Sprite(ref, renderToSurface(), m_currentPalette)));
        }
    }
}

}
}
