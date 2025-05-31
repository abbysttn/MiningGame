#ifndef UI_H
#define UI_H

#include "font.h"
#include "vector2.h"
#include "texture.h"
#include "sprite.h"
#include "player.h"
#include "renderer.h"

#include "SDL.h"
#include <memory>
#include <string>

class Renderer;

class UI {
public:
    UI(Renderer* renderer);
    ~UI();

    void Update(Player* player, Renderer* renderer);
    void Render();

    void LoadDefaultTexture();

protected:
    SDL_Color white = { 255, 255, 255, 255 };

private:
    Renderer* m_pRenderer;
    std::unique_ptr<Font> m_font;

	float m_screenWidth;
	float m_screenHeight;

    Texture m_depthTexture;
    std::unique_ptr<Sprite> m_depthSprite;

    std::string m_depthText;
    Vector2 m_depthTextPos;

    // Set health and stamina bar position and size
    const float barWidth = 200.0f;
    const float barHeight = 25.0f;
    Vector2 barPositionHealth;
    Vector2 barPositionStamina;

    float healthPercent;
    float staminaPercent;
};

#endif // UI_H