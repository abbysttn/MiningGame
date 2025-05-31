// COMP710 GP Framework 2025
#ifndef TEXTURE_H
#define TEXTURE_H

// Library includes:
#include <SDL.h>
#include "SDL_ttf.h"
#include "font.h"
#include <glew.h>

class Texture
{
	// Member methods:
public:
	Texture();
	~Texture();

	bool Initialise(const char* pcFilename);

	void LoadSurfaceIntoTexture(SDL_Surface* pSurface);


	void SetActive();

	int GetWidth() const;
	int GetHeight() const;

	unsigned int GetTextureId() const { return m_uiTextureId; }
	void SetID(GLuint id);

	void LoadTextTexture(SDL_Renderer* renderer, const char* text, const Font& font, SDL_Color color);

protected:

private:
	Texture(const Texture& texture);
	//Texture& operator=(const Texture& texture);

	// Member data:
public:

protected:
	unsigned int m_uiTextureId;
	int m_iWidth;
	int m_iHeight;

private:

};

#endif // TEXTURE_H
