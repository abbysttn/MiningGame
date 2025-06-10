// This includes
#include "sceneloadingscreen.h"

// Local icnludes
#include "renderer.h"
#include "inputsystem.h"
#include "sprite.h"
#include "texture.h"
#include "font.h"
#include "logmanager.h"

//IMGUI
#include "imgui/imgui.h"

// Constants
const char* FONT_FILEPATH = "../assets/fonts/joystix.otf";
const int FONT_SIZE = 24;
const SDL_Color TEXT_COLOR = { 255, 255, 255, 255 }; // White color
const char* TEXT_STRING = "LOADING";
const float FADE_DURATION = 1.0f;
const float DISPLAY_DURATION = 1.0f; // Stay on the screen for at least this long
const float DOT_ANIMATION_INTERVAL = 0.5f; // Interval for dot animation
const int TEXT_PADDING_FROM_EDGE = 20;

SceneLoadingScreen::SceneLoadingScreen()
    : m_pRenderer(nullptr)
    , m_pLoadingFont(nullptr)
    , m_pLoadingTextTexture(nullptr)
    , m_pLoadingTextSprite(nullptr)
    , m_bTextSpriteInitialised(false)
    , m_fFadeDuration(FADE_DURATION)
    , m_fMinDisplayDuration(DISPLAY_DURATION)
    , m_fTimer(0.0f)
    , m_fDisplayStateTimer(0.0f)
    , m_fDotAnimationTimer(0.0f)
    , m_fDotAnimationInterval(DOT_ANIMATION_INTERVAL)
    , m_fAlpha(0.0f)
    , m_iNumDots(1) // Start with 1 dot
    , m_sLoadingTextBase(TEXT_STRING)
    , m_textColor(TEXT_COLOR)
    , m_bActualLoadingFinished(false)
    , m_eCurrentState(LoadingState::IDLE)
{
}

SceneLoadingScreen::~SceneLoadingScreen()
{
    delete m_pLoadingTextSprite;
    m_pLoadingTextSprite = nullptr;

	delete m_pLoadingTextTexture;
    m_pLoadingTextTexture = nullptr;

	delete m_pLoadingFont;
	m_pLoadingFont = nullptr;
}

bool SceneLoadingScreen::Initialise(Renderer& renderer)
{
    m_pRenderer = &renderer;

	m_pLoadingFont = new Font(FONT_FILEPATH, FONT_SIZE);
    if (!m_pLoadingFont || !m_pLoadingFont->GetFont())
    {
		LogManager::GetInstance().Log("Failed to load loading font.");
		return false;
    }

    m_pLoadingTextTexture = new Texture();
    if (!m_pLoadingTextTexture)
    {
        LogManager::GetInstance().Log("Failed to allocate Texture for loading screen.");
        return false;
    }

    m_pLoadingTextSprite = new Sprite();
    if (!m_pLoadingTextSprite)
    {
        LogManager::GetInstance().Log("Failed to allocate Sprite for loading screen.");
        return false;
    }

    m_iNumDots = 3;
	UpdateLoadingText();

    m_eCurrentState = LoadingState::IDLE;
    m_fAlpha = 0.0f;
    if (m_pLoadingTextSprite)
    {
		m_pLoadingTextSprite->SetAlpha(m_fAlpha);
    }

    return true;
}

void SceneLoadingScreen::OnEnter()
{
    m_fTimer = 0.0f;
    m_fDisplayStateTimer = 0.0f;
    m_fDotAnimationTimer = 0.0f;
    m_fAlpha = 0.0f;
    m_iNumDots = 1; // Animation starts with 1 dot
    m_bActualLoadingFinished = false;
    m_eCurrentState = LoadingState::FADING_IN;

    if (m_pRenderer && m_pLoadingFont && m_pLoadingTextTexture && m_pLoadingTextSprite && m_bTextSpriteInitialised)
    {
		UpdateLoadingText();
    }
    else
    {
        LogManager::GetInstance().Log("SceneLoadingScreen::OnEnter() - Failed, Fade in is aborted.");
        m_eCurrentState = LoadingState::FINISHED;
        return;
    }

    if (m_pLoadingTextSprite)
    {
        m_pLoadingTextSprite->SetAlpha(m_fAlpha);
    }
}

void SceneLoadingScreen::UpdateLoadingText()
{
    if (!m_pRenderer || !m_pLoadingFont || !m_pLoadingTextTexture || !m_pLoadingTextSprite)
    {
        LogManager::GetInstance().Log("SceneLoadingScreen::UpdateLoadingText() called with uninitialised essential members.");
        return;
    }

    std::string textToShow = m_sLoadingTextBase;
    for (int i = 0; i < m_iNumDots; ++i)
    {
        textToShow += ".";
    }

    m_pLoadingTextTexture->LoadTextTexture(m_pRenderer->GetSDLRenderer(), textToShow.c_str(), *m_pLoadingFont, m_textColor);

    if (m_pLoadingTextTexture->GetWidth() == 0)
    {
        LogManager::GetInstance().Log("Failed to load text into texture in SceneLoadingScreen::UpdateLoadingText.");
        return;
    }

    if (!m_bTextSpriteInitialised)
    {
        if (!m_pLoadingTextSprite->Initialise(*m_pLoadingTextTexture))
        {
            LogManager::GetInstance().Log("Failed to initialise loading text sprite in SceneLoadingScreen::UpdateLoadingText.");
            return;
        }
        m_bTextSpriteInitialised = true;
    }

    int screenWidth = m_pRenderer->GetWidth();
    int screenHeight = m_pRenderer->GetHeight();
    int spriteWidth = m_pLoadingTextSprite->GetWidth();
    int spriteHeight = m_pLoadingTextSprite->GetHeight();

    // Text position
    // Set sprite to bottom right corner
    m_pLoadingTextSprite->SetX(screenWidth - (spriteWidth / 2) - TEXT_PADDING_FROM_EDGE);
    m_pLoadingTextSprite->SetY(screenHeight - (spriteHeight / 2) - TEXT_PADDING_FROM_EDGE);

    if (m_pLoadingTextSprite)
    {
		m_pLoadingTextSprite->SetAlpha(m_fAlpha);
    }
}

void SceneLoadingScreen::Process(float deltaTime, InputSystem& inputSystem)
{
    if (m_eCurrentState == LoadingState::IDLE || m_eCurrentState == LoadingState::FINISHED)
    {
        return;
    }

    m_fTimer += deltaTime;

    switch (m_eCurrentState)
    {
    case LoadingState::FADING_IN:
        m_fAlpha = m_fTimer / m_fFadeDuration;
        if (m_fAlpha >= 1.0f)
        {
            m_fAlpha = 1.0f;
            m_eCurrentState = LoadingState::DISPLAYING;
            m_fDisplayStateTimer = 0.0f;
            m_fDotAnimationTimer = 0.0f;
            m_iNumDots = 1;
            UpdateLoadingText();
        }
        break;

    case LoadingState::DISPLAYING:
        m_fDisplayStateTimer += deltaTime;
        m_fDotAnimationTimer += deltaTime;

        // Animate dots
        if (m_fDotAnimationTimer >= m_fDotAnimationInterval)
        {
            m_iNumDots = (m_iNumDots % 3) + 1; // Cycle through 1, 2, 3 dots
            UpdateLoadingText(); // Update texture and reposition sprite
            m_fDotAnimationTimer = 0.0f; // Reset dot animation timer
        }

        if (m_bActualLoadingFinished && m_fDisplayStateTimer >= m_fMinDisplayDuration)
        {
            m_eCurrentState = LoadingState::FADING_OUT;
            m_fTimer = 0.0f;
        }
        break;

    case LoadingState::FADING_OUT:
        m_fAlpha = 1.0f - (m_fTimer / m_fFadeDuration);
        if (m_fAlpha <= 0.0f)
        {
            m_fAlpha = 0.0f;
            m_eCurrentState = LoadingState::FINISHED;
        }
        break;

    case LoadingState::IDLE:
    case LoadingState::FINISHED:
        break;
    }

    if (m_pLoadingTextSprite)
    {
		m_pLoadingTextSprite->SetAlpha(m_fAlpha);
    }
}

void SceneLoadingScreen::Draw(Renderer& renderer)
{
    SDL_Renderer* sdlRenderer = renderer.GetSDLRenderer();
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);     // Opaque Black
    SDL_RenderClear(sdlRenderer);                          // Clear the entire screen to black

    if (m_pLoadingTextSprite && m_bTextSpriteInitialised &&
        (m_eCurrentState == LoadingState::FADING_IN ||
            m_eCurrentState == LoadingState::DISPLAYING ||
            m_eCurrentState == LoadingState::FADING_OUT))
    {
        m_pLoadingTextSprite->Draw(renderer);
    }
}

void SceneLoadingScreen::DebugDraw()
{

}

bool SceneLoadingScreen::IsFinished() const
{
	return m_eCurrentState == LoadingState::FINISHED;
}

void SceneLoadingScreen::SetActualLoadingComplete(bool complete)
{
    m_bActualLoadingFinished = complete;
}

bool SceneLoadingScreen::IsActualLoadingComplete() const
{
	return m_bActualLoadingFinished;
}

LoadingState SceneLoadingScreen::GetCurrentLoadingState() const
{
	return m_eCurrentState;
}