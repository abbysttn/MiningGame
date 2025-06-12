// THis include
#include "scenegameover.h"

// Local
#include "renderer.h"
#include "texture.h"
#include "sprite.h"
#include "inputsystem.h"
#include "game.h"
#include "font.h"
#include "logmanager.h"
#include "vector2.h"

// IMGUi
#include "imgui/imgui.h"

// Lib
#include <algorithm>

const int MAIN_SCENE_INDEX = 5;
const int TITLE_SCENE_INDEX = 3;

SceneGameOver::SceneGameOver()
    : m_pRestartBtnTexture(nullptr)
    , m_pRestartBtnSprite(nullptr)
    , m_pExitBtnTexture(nullptr)
    , m_pExitBtnSprite(nullptr)
    , m_pGameOverTextTexture(nullptr)
    , m_pGameOverTextSprite(nullptr)
    , m_pBackgroundSprite(nullptr)
    , m_pButtonFont(nullptr)
    , m_pTitleFont(nullptr)
    , m_bisMouseOverRestart(false)
    , m_bisMouseOverExit(false)
    , m_screenWidth(0.0f)
    , m_screenHeight(0.0f)
    , m_fadeAlpha(255.0f)
    , m_isFadingIn(true)
{
}

SceneGameOver::~SceneGameOver()
{
    delete m_pRestartBtnSprite;
    m_pRestartBtnSprite = nullptr;
    delete m_pExitBtnSprite; 
    m_pExitBtnSprite = nullptr;
    delete m_pGameOverTextSprite; 
    m_pGameOverTextSprite = nullptr;
    delete m_pBackgroundSprite; 
    m_pBackgroundSprite = nullptr;

    delete m_pRestartBtnTexture; 
    m_pRestartBtnTexture = nullptr;
    delete m_pExitBtnTexture; 
    m_pExitBtnTexture = nullptr;
    delete m_pGameOverTextTexture; 
    m_pGameOverTextTexture = nullptr;

    delete m_pButtonFont; 
    m_pButtonFont = nullptr;
    delete m_pTitleFont; 
    m_pTitleFont = nullptr;

    m_pSoundSystem.Release();
}

void SceneGameOver::OnEnter(bool reset)
{
    m_isFadingIn = true;
    m_fadeAlpha = 255.0f;
    Game::GetInstance().m_pInputSystem->ShowMouseCursor(true);

	Renderer* renderer = &Game::GetInstance().GetRenderer();
    if (renderer)
    {
		renderer->SetCameraPosition(m_screenWidth / 2.0f, m_screenHeight / 2.0f);
    }
}

bool SceneGameOver::Initialise(Renderer& renderer)
{
    m_pSoundSystem.Initialise();
    m_pSoundSystem.LoadSound("click", "../assets/sound/Button_Click.mp3");

    m_screenWidth = static_cast<float>(renderer.GetWidth());
    m_screenHeight = static_cast<float>(renderer.GetHeight());
    SDL_Renderer* sdlRenderer = renderer.GetSDLRenderer();

    // Background 
    m_pBackgroundSprite = renderer.CreateSprite("../assets/pauseBackground.png");
    m_pBackgroundSprite->SetX(static_cast<int>(m_screenWidth / 2));
    m_pBackgroundSprite->SetY(static_cast<int>(m_screenHeight / 2));
    float bgScaleX = m_screenWidth / m_pBackgroundSprite->GetWidth();
    float bgScaleY = m_screenHeight / m_pBackgroundSprite->GetHeight();
    m_pBackgroundSprite->SetScale(std::max(bgScaleX, bgScaleY));

    // Fonts
    const char* fontFilename = "../assets/fonts/joystix.otf";
    m_pButtonFont = new Font(fontFilename, 38);
    m_pTitleFont = new Font(fontFilename, 64);
    SDL_Color textColor = { 255, 255, 255, 255 };

    // "Game Over"
    m_pGameOverTextTexture = new Texture();
    m_pGameOverTextTexture->LoadTextTexture(sdlRenderer, "Game Over", *m_pTitleFont, textColor);
    m_pGameOverTextSprite = new Sprite();
    m_pGameOverTextSprite->Initialise(*m_pGameOverTextTexture);
    m_pGameOverTextSprite->SetX(static_cast<int>(m_screenWidth / 2));
    m_pGameOverTextSprite->SetY(static_cast<int>(m_screenHeight / 2 - 120));

    // Restart Button
    m_pRestartBtnTexture = new Texture();
    m_pRestartBtnTexture->LoadTextTexture(sdlRenderer, "Restart Game", *m_pButtonFont, textColor);
    m_pRestartBtnSprite = new Sprite();
    m_pRestartBtnSprite->Initialise(*m_pRestartBtnTexture);
    m_pRestartBtnSprite->SetX(static_cast<int>(m_screenWidth / 2));
    m_pRestartBtnSprite->SetY(static_cast<int>(m_screenHeight / 2));
    m_pRestartBtnSprite->SetRedTint(m_defaultRed); m_pRestartBtnSprite->SetGreenTint(m_defaultGreen); m_pRestartBtnSprite->SetBlueTint(m_defaultBlue);

    // Exit Button
    m_pExitBtnTexture = new Texture();
    m_pExitBtnTexture->LoadTextTexture(sdlRenderer, "Exit Game", *m_pButtonFont, textColor);
    m_pExitBtnSprite = new Sprite();
    m_pExitBtnSprite->Initialise(*m_pExitBtnTexture);
    m_pExitBtnSprite->SetX(static_cast<int>(m_screenWidth / 2));
    m_pExitBtnSprite->SetY(m_pRestartBtnSprite->GetY() + m_pRestartBtnTexture->GetHeight() + 30);
    m_pExitBtnSprite->SetRedTint(m_defaultRed); m_pExitBtnSprite->SetGreenTint(m_defaultGreen); m_pExitBtnSprite->SetBlueTint(m_defaultBlue);

    return true;
}

void SceneGameOver::Process(float deltaTime, InputSystem& inputSystem)
{
    // fade-in effect
    if (m_isFadingIn)
    {
        m_fadeAlpha -= FADE_SPEED * deltaTime;
        if (m_fadeAlpha <= 0.0f)
        {
            m_fadeAlpha = 0.0f;
            m_isFadingIn = false;
        }
    }

    if (m_isFadingIn) return;

    // Restart button logic
    m_bisMouseOverRestart = IsMouseOverSprite(inputSystem, m_pRestartBtnSprite);
    if (m_bisMouseOverRestart)
    {
        m_pRestartBtnSprite->SetRedTint(m_hoverRed); m_pRestartBtnSprite->SetGreenTint(m_hoverGreen); m_pRestartBtnSprite->SetBlueTint(m_hoverBlue);
        if (inputSystem.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED)
        {
            m_pSoundSystem.PlaySound("click");
            Game::GetInstance().SetCurrentScene(MAIN_SCENE_INDEX); // This will call OnEnter with reset=true
        }
    }
    else
    {
        m_pRestartBtnSprite->SetRedTint(m_defaultRed); m_pRestartBtnSprite->SetGreenTint(m_defaultGreen); m_pRestartBtnSprite->SetBlueTint(m_defaultBlue);
    }

    // Exit button logic
    m_bisMouseOverExit = IsMouseOverSprite(inputSystem, m_pExitBtnSprite);
    if (m_bisMouseOverExit)
    {
        m_pExitBtnSprite->SetRedTint(m_hoverRed); m_pExitBtnSprite->SetGreenTint(m_hoverGreen); m_pExitBtnSprite->SetBlueTint(m_hoverBlue);
        if (inputSystem.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED)
        {
            m_pSoundSystem.PlaySound("click");
            Game::GetInstance().Quit();
        }
    }
    else
    {
        m_pExitBtnSprite->SetRedTint(m_defaultRed); m_pExitBtnSprite->SetGreenTint(m_defaultGreen); m_pExitBtnSprite->SetBlueTint(m_defaultBlue);
    }

    m_pSoundSystem.Update();
}

void SceneGameOver::Draw(Renderer& renderer)
{
    if (m_pBackgroundSprite) m_pBackgroundSprite->Draw(renderer);
    if (m_pGameOverTextSprite) m_pGameOverTextSprite->Draw(renderer);
    if (m_pRestartBtnSprite) m_pRestartBtnSprite->Draw(renderer);
    if (m_pExitBtnSprite) m_pExitBtnSprite->Draw(renderer);

    if (m_isFadingIn)
    {
        SDL_SetRenderDrawBlendMode(renderer.GetSDLRenderer(), SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer.GetSDLRenderer(), 0, 0, 0, static_cast<Uint8>(m_fadeAlpha));
        SDL_Rect screenRect = { 0, 0, (int)m_screenWidth, (int)m_screenHeight };
        SDL_RenderFillRect(renderer.GetSDLRenderer(), &screenRect);
    }
}

void SceneGameOver::DebugDraw()
{
    ImGui::Text("Scene: Game Over");
}

bool SceneGameOver::IsMouseOverSprite(InputSystem& inputSystem, Sprite* sprite)
{
    if (!sprite) return false;
    const Vector2& mousePos = inputSystem.GetMousePosition();
    int mouseX = static_cast<int>(mousePos.x);
    int mouseY = static_cast<int>(mousePos.y);
    int spriteX = sprite->GetX();
    int spriteY = sprite->GetY();
    int spriteWidth = sprite->GetWidth();
    int spriteHeight = sprite->GetHeight();
    float halfWidth = static_cast<float>(spriteWidth) / 2.0f;
    float halfHeight = static_cast<float>(spriteHeight) / 2.0f;
    return (mouseX >= spriteX - halfWidth &&
        mouseX <= spriteX + halfWidth &&
        mouseY >= spriteY - halfHeight &&
        mouseY <= spriteY + halfHeight);
}