#ifndef __SCENEGAMEOVER_H__
#define __SCENEGAMEOVER_H__

#include "scene.h"
#include "soundsystem.h"
#include "game.h"

class Sprite;
class Texture;
class Renderer;
class InputSystem;
class Font;

class SceneGameOver : public Scene
{
public:
    SceneGameOver();
    virtual ~SceneGameOver();

    virtual bool Initialise(Renderer& renderer);
    virtual void Process(float deltaTime, InputSystem& inputSystem);
    virtual void Draw(Renderer& renderer);
    virtual void DebugDraw();

    void OnEnter(bool reset) override;

private:
    SceneGameOver(const SceneGameOver& scene);
    SceneGameOver& operator=(const SceneGameOver& scene);

    bool IsMouseOverSprite(InputSystem& inputSystem, Sprite* sprite);

protected:
    // Buttons
    Texture* m_pRestartBtnTexture;
    Sprite* m_pRestartBtnSprite;
    Texture* m_pExitBtnTexture;
    Sprite* m_pExitBtnSprite;
    Texture* m_pGameOverTextTexture;
    Sprite* m_pGameOverTextSprite;


    // Visuals
    Sprite* m_pBackgroundSprite;
    Font* m_pButtonFont;
    Font* m_pTitleFont;

    bool m_bisMouseOverRestart;
    bool m_bisMouseOverExit;

    float m_screenWidth;
    float m_screenHeight;

    // Transition 
    float m_fadeAlpha;
    bool m_isFadingIn;
    const float FADE_SPEED = 200.0f;

    SoundSystem m_pSoundSystem;

    // Color constants
    const float m_defaultRed = 1.0f, m_defaultGreen = 1.0f, m_defaultBlue = 1.0f;
    const float m_hoverRed = 1.0f, m_hoverGreen = 0.65f, m_hoverBlue = 0.0f;
};

#endif // __SCENEGAMEOVER_H__