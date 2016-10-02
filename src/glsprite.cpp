#include "glsprite.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

#include <SDL2/SDL_image.h>

namespace Nugem {

GlSpriteCollection::GlSpriteCollection(GLuint tid, std::vector<GlSpriteCollectionData> &&spriteList): m_tid(tid), m_sprites(spriteList)
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

void Nugem::GlSpriteCollection::display(GlGraphics &glGraphics, size_t index, SDL_Rect &location)
{
	glGraphics.atlasTid = m_tid;
	glGraphics.positionVertice.push_back({ { location.x, location.y } });
	glGraphics.positionVertice.push_back({ { location.x + location.w, location.y } });
	glGraphics.positionVertice.push_back({ { location.x, location.y + location.h } });
	glGraphics.positionVertice.push_back({ { location.x + location.w, location.y + location.h } });
	glGraphics.positionVertice.push_back({ { location.x + location.w, location.y } });
	glGraphics.positionVertice.push_back({ { location.x, location.y + location.h } });
	glGraphics.texCoords.push_back({{ static_cast<GLfloat>(m_sprites[index].x) / m_totalWidth, 0 }});
	glGraphics.texCoords.push_back({{ static_cast<GLfloat>(m_sprites[index].x + m_sprites[index].w) / m_totalWidth, 0 }});
	glGraphics.texCoords.push_back({{ static_cast<GLfloat>(m_sprites[index].x) / m_totalWidth, static_cast<GLfloat>(m_sprites[index].h) / m_totalHeight }});
	glGraphics.texCoords.push_back({{ static_cast<GLfloat>(m_sprites[index].x + m_sprites[index].w) / m_totalWidth, static_cast<GLfloat>(m_sprites[index].h) / m_totalHeight }});
	glGraphics.texCoords.push_back({{ static_cast<GLfloat>(m_sprites[index].x + m_sprites[index].w) / m_totalWidth, 0 }});
	glGraphics.texCoords.push_back({{ static_cast<GLfloat>(m_sprites[index].x) / m_totalWidth, static_cast<GLfloat>(m_sprites[index].h) / m_totalHeight }});
}

GlSpriteCollectionBuilder::GlSpriteCollectionBuilder(): m_maxHeight(0), m_totalWidth(0), m_built(false), m_result(nullptr)
{
	glActiveTexture(GL_TEXTURE0);
	testGlError();
	glGenTextures(1, &m_tid);
	glBindTexture(GL_TEXTURE_2D, m_tid);
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
	
	testGlError();
}

GlSpriteCollectionBuilder::~GlSpriteCollectionBuilder()
{
	if (!m_built) {
		glDeleteTextures(1, &m_tid);
	}
}

size_t GlSpriteCollectionBuilder::addSprite(const SDL_Surface *surface)
{
	if (surface->h > m_maxHeight)
		m_maxHeight = surface->h;
	GLuint currentOrdinate = m_totalWidth;
	m_totalWidth += surface->w;
	testGlError();
// 	// Resize the texture atlas to fit the new surface
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_totalWidth, m_maxHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	testGlError();
// 	// Add the new surface in the free space
	glTexSubImage2D(GL_TEXTURE_2D, 0, currentOrdinate, 0, surface->w, surface->h, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	testGlError();
	size_t identifier = m_spriteList.size();
	m_spriteList.push_back({ (size_t) surface->w,  (size_t) surface->h,  (size_t) currentOrdinate });
	IMG_SavePNG(const_cast<SDL_Surface *>(surface), ("img_" + std::to_string(identifier) + ".png").c_str()); // DEBUG
	return identifier;
}

GlSpriteCollection *GlSpriteCollectionBuilder::build()
{
	if (!m_built || !m_result) {
		glGenerateMipmap(GL_TEXTURE_2D);
		m_result = new GlSpriteCollection(m_tid, std::move(m_spriteList));
		m_built = true;
		testGlError();
	}
	return m_result;
}


}
