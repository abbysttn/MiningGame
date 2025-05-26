#ifndef UI_H
#define UI_H

#include "font.h"
#include "vector2.h"
#include "texture.h"
#include "sprite.h"

#include "SDL.h"
#include <memory>
#include <string>

class Renderer;

class UI {
public:
    UI(Renderer* renderer);
    ~UI();

    void Update(int currentDepth);
    void Render();

    void LoadDefaultTexture();

protected:
    SDL_Color white = { 255, 255, 255, 255 };

private:
    Renderer* m_SDLRenderer;
    std::unique_ptr<Font> m_font;

    Texture m_depthTexture;
    std::unique_ptr<Sprite> m_depthSprite;

    std::string m_depthText;
    Vector2 m_depthTextPos;
};

#endif // UI_H