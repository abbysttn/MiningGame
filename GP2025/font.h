#ifndef FONT_H
#define FONT_H

#include "SDL_ttf.h"

class Font {
public:
	Font(const char* filename, int size);
	~Font();

	TTF_Font* GetFont() const;

private:
	TTF_Font* m_font;
};

#endif // FONT_H
