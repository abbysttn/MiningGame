#ifndef __SCENELOADINGSCREEN_H__
#define __SCENELOADINGSCREEN_H__

// Lib includes
#include <SDL.h>
#include <string>

// Local includes
#include "Scene.h"

// Forward declarations
class Renderer;
class InputSystem;
class Sprite;
class Texture;
class Font;

enum class LoadingState
{
    IDLE,
    FADING_IN,
    DISPLAYING,
    FADING_OUT,
    FINISHED
};

class SceneLoadingScreen : public Scene
{
    // Member methods
public:
    SceneLoadingScreen();
    virtual ~SceneLoadingScreen();

    virtual bool Initialise(Renderer& renderer);
    virtual void Process(float deltaTime, InputSystem& inputSystem);
    virtual void Draw(Renderer& renderer);
    virtual void DebugDraw();

    virtual void OnEnter(); 

    bool IsFinished() const;
    void SetActualLoadingComplete(bool complete);
    bool IsActualLoadingComplete() const;
    LoadingState GetCurrentLoadingState() const;

private:
    // Helper to update the text texture and sprite position
    void UpdateLoadingText();

    // Member data
private:
    Renderer* m_pRenderer;

    Font* m_pLoadingFont;
    Texture* m_pLoadingTextTexture;
    Sprite* m_pLoadingTextSprite;
    bool m_bTextSpriteInitialised;

    float m_fFadeDuration;        
    float m_fMinDisplayDuration;  

    float m_fTimer;               
    float m_fDisplayStateTimer;   
    float m_fDotAnimationTimer;   
    float m_fDotAnimationInterval;

    float m_fAlpha;               
    int m_iNumDots;             
    std::string m_sLoadingTextBase; // The base string, e.g., "LOADING"
    SDL_Color m_textColor;        // Color of the loading text

    bool m_bActualLoadingFinished; // Flag set by Game class when background loading is complete

    LoadingState m_eCurrentState;

    SceneLoadingScreen(const SceneLoadingScreen&);
    SceneLoadingScreen& operator=(const SceneLoadingScreen&);
};

#endif // !__SCENELOADINGSCREEN_H__