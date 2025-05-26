#include "font.h"
#include "logmanager.h"

Font::Font(const char* filename, int size)
{
	m_font = TTF_OpenFont(filename, size);
	if (!m_font) {
		LogManager::GetInstance().Log("Failed to load font.");
	}
}

Font::~Font() {
	if (m_font) {
		TTF_CloseFont(m_font);
		m_font = nullptr;
	}
}

TTF_Font* Font::GetFont() const {
	return m_font;
}

