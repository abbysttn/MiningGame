#ifndef __SCENEKEYBOARDCONTROLS_H__
#define __SCENEKEYBOARDCONTROLS_H__

#include "scene.h"
#include "soundsystem.h"
#include <vector>
#include <SDL_pixels.h>

class Sprite;
class Texture;
class Renderer;
class InputSystem;
class Font;

class SceneKeyboardControls : public Scene
{
public:
    SceneKeyboardControls();
    virtual ~SceneKeyboardControls();

    virtual bool Initialise(Renderer& renderer);
    virtual void Process(float deltaTime, InputSystem& inputSystem);
    virtual void Draw(Renderer& renderer);
    virtual void DebugDraw();

private:
    SceneKeyboardControls(const SceneKeyboardControls& scene);
    SceneKeyboardControls& operator=(const SceneKeyboardControls& scene);

    bool IsMouseOverSprite(InputSystem& inputSystem, Sprite* sprite);
    void CreateControlLine(Renderer& renderer, Font& font, SDL_Color color, const char* text, const std::vector<const char*>& spriteFiles, int yPos, std::vector<Sprite*>& outSprites, std::vector<Texture*>& outTextures);

protected:
    // Back button
    Texture* m_pBackBtnTexture;
    Sprite* m_pBackBtnSprite;

    // Visuals
    Sprite* m_pBackgroundSprite;
    Font* m_pTextFont;

    // All text textures and sprites for controls
    std::vector<Texture*> m_controlTextures;
    std::vector<Sprite*> m_controlSprites;

    // State
    bool m_bisMouseOverBack;
    float m_screenWidth;
    float m_screenHeight;

    SoundSystem m_pSoundSystem;

    const float m_defaultRed = 1.0f, m_defaultGreen = 1.0f, m_defaultBlue = 1.0f;
    const float m_hoverRed = 1.0f, m_hoverGreen = 0.65f, m_hoverBlue = 0.0f;
};

#endif // __SCENEKEYBOARDCONTROLS_H__