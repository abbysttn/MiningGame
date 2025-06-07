// This include
#include "scenepause.h"

// Local includes
#include "renderer.h"
#include "texture.h"
#include "sprite.h"
#include "inputsystem.h"
#include "game.h"
#include "logmanager.h"
#include "font.h"
#include "vector2.h"

// IMGUI stuffz
#include "imgui/imgui.h"
#include <algorithm>
#include <iostream>

const int TITLE_SCENE_INDEX = 2;

ScenePause::ScenePause()
    : m_pResumeBtnTexture(nullptr)
    , m_pQuitToTitleBtnTexture(nullptr)
    , m_pExitGameBtnTexture(nullptr)
    , m_pResumeBtnSprite(nullptr)
    , m_pQuitToTitleBtnSprite(nullptr)
    , m_pExitGameBtnSprite(nullptr)
    , m_pButtonFont(nullptr)
    , m_screenWidth(0.0f)
    , m_screenHeight(0.0f)
{
}

ScenePause::~ScenePause()
{
    delete m_pResumeBtnSprite;
    m_pResumeBtnSprite = nullptr;
    delete m_pQuitToTitleBtnSprite;
    m_pQuitToTitleBtnSprite = nullptr;
    delete m_pExitGameBtnSprite;
    m_pExitGameBtnSprite = nullptr;

    delete m_pResumeBtnTexture;
    m_pResumeBtnTexture = nullptr;
    delete m_pQuitToTitleBtnTexture;
    m_pQuitToTitleBtnTexture = nullptr;
    delete m_pExitGameBtnTexture;
    m_pExitGameBtnTexture = nullptr;

    delete m_pButtonFont;
    m_pButtonFont = nullptr;

    m_soundSystem.Release();
}

bool ScenePause::Initialise(Renderer& renderer)
{
    m_screenWidth = static_cast<float>(renderer.GetWidth());
    m_screenHeight = static_cast<float>(renderer.GetHeight());

    if (!m_soundSystem.Initialise())
    {
        LogManager::GetInstance().Log("Pause scene failed to initialise sound system!!");
    }

    m_soundSystem.LoadSound("click", "../assets/sound/Button_Click.mp3");

    SDL_Renderer* sdlRenderer = renderer.GetSDLRenderer();

    if (Game::GetInstance().m_pInputSystem)
    {
		Game::GetInstance().m_pInputSystem->ShowMouseCursor(true);
		Game::GetInstance().m_pInputSystem->SetRelativeMode(false);
    }
    
    // FOnt
    const char* fontFilename = "../game/silkscreen.ttf";
    int buttonFontSize = 32;
	m_pButtonFont = new Font(fontFilename, buttonFontSize);
	SDL_Color textColor = { 255, 255, 255, 255 }; // White color

    // Resume Button
	m_pResumeBtnTexture = new Texture();
	m_pResumeBtnTexture->LoadTextTexture(sdlRenderer, "Resume", *m_pButtonFont, textColor);
	if (m_pResumeBtnTexture->GetWidth() == 0)
	{
		LogManager::GetInstance().Log("Failed to load resume button");
		delete m_pResumeBtnTexture;
		m_pResumeBtnTexture = nullptr;
		return false;
	}

	m_pResumeBtnSprite = new Sprite();
	if (!m_pResumeBtnSprite->Initialise(*m_pResumeBtnTexture))
	{
		LogManager::GetInstance().Log("Failed to initialise resume button sprite");
		delete m_pResumeBtnSprite;
		m_pResumeBtnSprite = nullptr;
		delete m_pResumeBtnTexture;
		m_pResumeBtnTexture = nullptr;
		return false;
	}
    m_pResumeBtnSprite->SetX(static_cast<int>(m_screenWidth / 2));
    m_pResumeBtnSprite->SetY(static_cast<int>(m_screenHeight * m_firstButtonYPosFactor));
    m_pResumeBtnSprite->SetRedTint(m_defaultRed);
    m_pResumeBtnSprite->SetGreenTint(m_defaultGreen);
    m_pResumeBtnSprite->SetBlueTint(m_defaultBlue);

	// Quit to Title Button
    m_pQuitToTitleBtnTexture = new Texture();
	m_pQuitToTitleBtnTexture->LoadTextTexture(sdlRenderer, "Quit to Title", *m_pButtonFont, textColor);
    if (m_pQuitToTitleBtnTexture->GetWidth() == 0)
    {
		LogManager::GetInstance().Log("Failed to load quit to title button");
		delete m_pQuitToTitleBtnTexture;
		m_pQuitToTitleBtnTexture = nullptr;
		return false;
    }

    m_pQuitToTitleBtnSprite = new Sprite();
    if (!m_pQuitToTitleBtnSprite->Initialise(*m_pQuitToTitleBtnTexture))
    {
		LogManager::GetInstance().Log("Failed to initialise quit to title button sprite");
		delete m_pQuitToTitleBtnSprite;
		m_pQuitToTitleBtnSprite = nullptr;
		delete m_pQuitToTitleBtnTexture;
		m_pQuitToTitleBtnTexture = nullptr;
		return false;
    }
    m_pQuitToTitleBtnSprite->SetX(static_cast<int>(m_screenWidth / 2));
    m_pQuitToTitleBtnSprite->SetY(m_pResumeBtnSprite->GetY() + static_cast<int>(m_buttonYOffset)); // Position relative to previous
    m_pQuitToTitleBtnSprite->SetRedTint(m_defaultRed);
    m_pQuitToTitleBtnSprite->SetGreenTint(m_defaultGreen);
    m_pQuitToTitleBtnSprite->SetBlueTint(m_defaultBlue);

	// Exit Game Button
    m_pExitGameBtnTexture = new Texture();
    m_pExitGameBtnTexture->LoadTextTexture(sdlRenderer, "Exit Game", *m_pButtonFont, textColor);
    if (m_pExitGameBtnTexture->GetWidth() == 0)
    {
        LogManager::GetInstance().Log("Failed to load Exit Game button text (or texture empty).");
        delete m_pExitGameBtnTexture;
        m_pExitGameBtnTexture = nullptr;
        return false;
    }

    m_pExitGameBtnSprite = new Sprite();
    if (!m_pExitGameBtnSprite->Initialise(*m_pExitGameBtnTexture))
    {
        LogManager::GetInstance().Log("Failed to initialise Exit Game button sprite.");
        delete m_pExitGameBtnSprite;
        m_pExitGameBtnSprite = nullptr;
        delete m_pExitGameBtnTexture;
        m_pExitGameBtnTexture = nullptr;
        return false;
    }
    m_pExitGameBtnSprite->SetX(static_cast<int>(m_screenWidth / 2));
    m_pExitGameBtnSprite->SetY(m_pQuitToTitleBtnSprite->GetY() + static_cast<int>(m_buttonYOffset)); // Position relative to previous
    m_pExitGameBtnSprite->SetRedTint(m_defaultRed);
    m_pExitGameBtnSprite->SetGreenTint(m_defaultGreen);
    m_pExitGameBtnSprite->SetBlueTint(m_defaultBlue);

    return true;
}

void ScenePause::Draw(Renderer& renderer)
{
    if (!m_bIsInitialised) return;

    renderer.DrawRectScreenSpace(
        Vector2(m_screenWidth / 2.0f, m_screenHeight / 2.0f),
        Vector2(m_screenWidth, m_screenHeight),
        0.05f, 0.05f, 0.05f, 0.80f
    );

    if (m_pResumeBtnSprite)
    {
        renderer.DrawSpriteScreenSpace(*m_pResumeBtnSprite);
    }

    if (m_pQuitToTitleBtnSprite)
    {
        renderer.DrawSpriteScreenSpace(*m_pQuitToTitleBtnSprite);
    }

    if (m_pExitGameBtnSprite)
    {
        renderer.DrawSpriteScreenSpace(*m_pExitGameBtnSprite);
    }
}

void ScenePause::Process(float deltaTime, InputSystem& inputSystem)
{
    if (!Scene::IsInitialised()) return;

	bool bIsMouseOverResume = false;
	bool bIsMouseOverQuitToTitle = false;
	bool bIsMouseOverExitGame = false;

    // Resume
    if (m_pResumeBtnSprite)
    {
		bIsMouseOverResume = IsMouseOverSprite(inputSystem, m_pResumeBtnSprite);
		SetButtonTints(m_pResumeBtnSprite, bIsMouseOverResume);

		if (bIsMouseOverResume && inputSystem.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED)
		{
			m_soundSystem.PlaySound("click");
			Game::GetInstance().Resume
		}
    }
}