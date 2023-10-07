#include "glsprite.hpp"

#include <iostream>

#include <SDL2/SDL_image.h>

namespace Nugem {
	
const SDL_Rect GlSpriteDisplayer::defaultSpriteCanvas = {-1, -1, -1, -1};

GlSpriteCollection::GlSpriteCollection(GLuint tid, std::vector<GlSpriteCollectionData> &&spriteList): m_tid(tid), m_sprites(spriteList), m_totalHeight(0)
{
	m_totalWidth = m_sprites.back().x + m_sprites.back().w;
	for (auto &sprite: m_sprites) {
		if (sprite.h > m_totalHeight)
			m_totalHeight = sprite.h;
	}
}

GlSpriteCollection::GlSpriteCollection(GlSpriteCollection &&original): m_tid(std::move(original.m_tid)), m_sprites(std::move(original.m_sprites)), m_totalWidth(std::move(original.m_totalWidth)), m_totalHeight(std::move(original.m_totalHeight))
{
	original.m_tid = 0;
}

GlSpriteCollection::~GlSpriteCollection()
{
	if (m_tid) {
		glDeleteTextures(1, &m_tid);
	}
}

GlSpriteCollectionBuilder::GlSpriteCollectionBuilder(): m_maxHeight(0), m_totalWidth(0), m_built(false), m_surface(nullptr), m_result(nullptr)
{
}

GlSpriteCollectionBuilder::~GlSpriteCollectionBuilder()
{
	if (m_surface)
		SDL_FreeSurface(m_surface);
}

size_t GlSpriteCollectionBuilder::addSprite(const SDL_Surface *surface)
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
	if ((long unsigned int) surface->h > m_maxHeight)
		m_maxHeight = surface->h;
	GLuint currentOrdinate = m_totalWidth;
	m_totalWidth += surface->w;
	testGlError();
	SDL_Surface *newSurface = SDL_CreateRGBSurface(0, m_totalWidth, m_maxHeight, 32, rmask, gmask, bmask, amask);
	if (m_surface) {
		SDL_Rect dstRect = { 0, 0, m_surface->w, m_surface->h };
		SDL_BlitSurface(m_surface, nullptr, newSurface, &dstRect);
		SDL_FreeSurface(m_surface);
	}
	{
		SDL_Rect dstRect = { 0, 0, surface->w, surface->h };
		dstRect.x = currentOrdinate;
		SDL_BlitSurface(const_cast<SDL_Surface *>(surface), nullptr, newSurface, &dstRect);
	}
	m_surface = newSurface;
	size_t identifier = m_spriteList.size();
	m_spriteList.push_back({ (size_t) surface->w,  (size_t) surface->h,  (size_t) currentOrdinate });
// 	IMG_SavePNG(const_cast<SDL_Surface *>(surface), ("img_" + std::to_string(identifier) + ".png").c_str()); // DEBUG
	return identifier;
}

GlSpriteCollection *GlSpriteCollectionBuilder::build()
{
	if (!m_built || !m_result) {
		testGlError();
		GLuint tid = 0;
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &tid);
		glBindTexture(GL_TEXTURE_2D, tid);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		
		testGlError();
		float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
	// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_surface->w, m_surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_surface->pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
		m_result = new GlSpriteCollection(tid, std::move(m_spriteList));
		m_built = true;
		testGlError();
	}
	return m_result;
}

GlSpriteDisplayer::GlSpriteDisplayer(GlSpriteCollection &spriteAtlas): m_spriteAtlas(spriteAtlas)
{}

void GlSpriteDisplayer::addSprite(size_t spriteNumber, const SDL_Rect &dest, const SDL_Rect &src)
{
	if (spriteNumber >= m_spriteAtlas.sprites().size()) {
		std::cerr << "Error: trying to display a sprite that is not on the atlas" << std::endl;
		return;
	}
	if (dest.w > 0 && dest.h > 0) {
		const GlSpriteCollectionData &sprite = m_spriteAtlas.sprites()[spriteNumber];
		bool enableDisplay = true;
		GLfloat bndLeft, bndRight;
		if (m_spriteAtlas.width()) {
			// left boundary
			bndLeft = static_cast<GLfloat>(sprite.x) / m_spriteAtlas.width();
			// right boundary
			bndRight = static_cast<GLfloat>(sprite.x + sprite.w) / m_spriteAtlas.width();
			if (src.x > 0 || src.w > 0) {
				GLfloat maxBndRight = bndRight;
				if (src.x > 0)
					bndLeft += src.x;
				if (src.w > 0)
					bndRight = bndLeft + src.w;
				if (bndLeft > maxBndRight)
					bndLeft = maxBndRight;
				if (bndRight > maxBndRight)
					bndRight = maxBndRight;
				
			}
		}
		GLfloat bndTop, bndBottom;
		// top boundary
		bndTop = 0;
		// bottom boundary
		if (m_spriteAtlas.height()) {
			bndBottom = static_cast<GLfloat>(sprite.h) / m_spriteAtlas.height();
			if (src.y > 0 || src.h > 0) {
				GLfloat maxBndBottom = bndBottom;
				if (src.y > 0)
					bndTop += src.y;
				if (src.h > 0)
					bndBottom = bndTop + src.h;
				if (bndBottom >= maxBndBottom)
					bndBottom = maxBndBottom;
				if (bndTop >= maxBndBottom)
					bndTop = maxBndBottom;
			}
		}
		if (enableDisplay) {
			m_positions.push_back({ { dest.x, dest.y } });
			m_positions.push_back({ { dest.x + dest.w, dest.y } });
			m_positions.push_back({ { dest.x, dest.y + dest.h } });
			m_positions.push_back({ { dest.x + dest.w, dest.y + dest.h } });
			m_positions.push_back({ { dest.x + dest.w, dest.y } });
			m_positions.push_back({ { dest.x, dest.y + dest.h } });
			m_texCoords.push_back({{ bndLeft, bndTop }});
			m_texCoords.push_back({{ bndRight, bndTop }});
			m_texCoords.push_back({{ bndLeft, bndBottom }});
			m_texCoords.push_back({{ bndRight, bndBottom }});
			m_texCoords.push_back({{ bndRight, bndTop }});
			m_texCoords.push_back({{ bndLeft, bndBottom }});
		}
	}
}

void GlSpriteDisplayer::display(GlGraphics &glGraphics)
{
	glGraphics.passItem(m_spriteAtlas.tid(), std::move(m_positions), std::move(m_texCoords));
}

}
