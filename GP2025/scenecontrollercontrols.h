#ifndef __SCENECONTROLLERCONTROLS_H__
#define __SCENECONTROLLERCONTROLS_H__

// Local includes
#include "scene.h"
#include "soundsystem.h"
#include <vector>
#include <SDL_pixels.h>

// Forward declarations:
class Sprite;
class Texture;
class Renderer;
class InputSystem;
class Font;

class SceneControllerControls : public Scene
{
public:
    SceneControllerControls();
    virtual ~SceneControllerControls();

    virtual bool Initialise(Renderer& renderer);
    virtual void Process(float deltaTime, InputSystem& inputSystem);
    virtual void Draw(Renderer& renderer);
    virtual void DebugDraw();

private:
    SceneControllerControls(const SceneControllerControls& scene);
    SceneControllerControls& operator=(const SceneControllerControls& scene);

    bool IsMouseOverSprite(InputSystem& inputSystem, Sprite* sprite);
    void CreateControlLine(Renderer& renderer, Font& font, SDL_Color color, const char* text, const std::vector<const char*>& spriteFiles, int yPos, std::vector<Sprite*>& outSprites, std::vector<Texture*>& outTextures);
    void CreateControlLineWithText(Renderer& renderer, Font& font, SDL_Color color, const char* labelText, const char* descriptionText, int yPos, std::vector<Sprite*>& outSprites, std::vector<Texture*>& outTextures);

protected:
    // Back button
    Texture* m_pBackBtnTexture;
    Sprite* m_pBackBtnSprite;

    // Visuals
    Sprite* m_pBackgroundSprite;
    Font* m_pTextFont;

    // All text textures and sprites for controller controls
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

#endif // __SCENECONTROLLERCONTROLS_H__