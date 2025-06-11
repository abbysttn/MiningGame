// THis includes
#include "scenetitlescreen.h"

// Local includes
#include "renderer.h"
#include "texture.h"
#include "sprite.h"
#include "inputsystem.h"
#include "game.h"
#include "logmanager.h"
#include "vector2.h"
#include "xboxcontroller.h"



// Lib includes
#include "imgui/imgui.h"
#include <algorithm>
#include <iostream>

const int LOADING_INDEX = 4;

SceneTitlescreen::SceneTitlescreen(FMOD::System* fmodSystem)
	: m_pStartBtnTexture(nullptr)
	, m_pExitBtnTexture(nullptr)
	, m_pStartBtnSprite(nullptr)
	, m_pExitBtnSprite(nullptr)
	, m_pBackgroundSprite(nullptr)
	, m_pTitleSprite(nullptr)
	, m_bisMouseOverStart(false)
	, m_bisMouseOverExit(false)
	, m_screenWidth(0.0f)
	, m_screenHeight(0.0f)
	, m_pFMODSystem(fmodSystem)
	, m_pClickSound(nullptr)
	, m_pBgmChannel(nullptr)
	, m_pBgmSound(nullptr)
	, m_pButtonFont(nullptr)
{
}

SceneTitlescreen::~SceneTitlescreen()
{
	delete m_pStartBtnSprite;
	m_pStartBtnSprite = nullptr;

	delete m_pExitBtnSprite;
	m_pExitBtnSprite = nullptr;

	delete m_pBackgroundSprite;
	m_pBackgroundSprite = nullptr;

	delete m_pTitleSprite;
	m_pTitleSprite = nullptr;

	delete m_pStartBtnTexture;
	m_pStartBtnTexture = nullptr;

	delete m_pExitBtnTexture;
	m_pExitBtnTexture = nullptr;

	delete m_pButtonFont;
	m_pButtonFont = nullptr;

	m_tSoundSystem.Release();
}

void SceneTitlescreen::OnEnter(bool reset)
{
	m_tSoundSystem.PlaySound("bgm");

}

void SceneTitlescreen::OnExit()
{
	m_tSoundSystem.StopSound("bgm");

}

bool SceneTitlescreen::Initialise(Renderer& renderer)
{

	if (!m_tSoundSystem.Initialise()) 
	{
		std::cerr << "Failed to initialise FMOD system!" << std::endl;
		return false;
	}
	m_tSoundSystem.LoadSound("click", "../assets/sound/Button_Click.mp3");
	m_tSoundSystem.LoadSound("bgm", "../assets/sound/BGM_Titlescreen.mp3", true);

	m_screenWidth = static_cast<float>(renderer.GetWidth());
	m_screenHeight = static_cast<float>(renderer.GetHeight());

	Game::GetInstance().m_pInputSystem->ShowMouseCursor(true);

	SDL_Renderer* sdlRenderer = renderer.GetSDLRenderer();

	// Background sprite
	m_pBackgroundSprite = renderer.CreateSprite("../assets/titleBackground.png");
	m_pBackgroundSprite->SetX(static_cast<int>(m_screenWidth / 2));
	m_pBackgroundSprite->SetY(static_cast<int>(m_screenHeight / 2));
	float bgScaleX = m_screenWidth / m_pBackgroundSprite->GetWidth();
	float bgScaleY = m_screenHeight / m_pBackgroundSprite->GetHeight();
	m_pBackgroundSprite->SetScale(std::max(bgScaleX, bgScaleY));

	// Title sprite
	m_pTitleSprite = renderer.CreateSprite("../assets/titleScreen.png");
	m_pTitleSprite->SetX(static_cast<int>(m_screenWidth / 2));
	m_pTitleSprite->SetY(static_cast<int>(m_screenHeight * m_titleTopMargin));

	// Font
	const char* fontFilename = "../game/silkscreen.ttf";
	int buttonFontSize = 38;

	m_pButtonFont = new Font(fontFilename, buttonFontSize);
	SDL_Color textColor = { 255, 255, 255, 255 };

	// Buttonz
	m_pStartBtnTexture = new Texture();
	m_pStartBtnTexture->LoadTextTexture(sdlRenderer, "Start Game", *m_pButtonFont, textColor);
	if (m_pStartBtnTexture->GetWidth() == 0)
	{
		LogManager::GetInstance().Log("Failed to load start game button");
		delete m_pStartBtnTexture;
		m_pStartBtnTexture = nullptr;
		return false;
	}

	m_pStartBtnSprite = new Sprite();
	if (!m_pStartBtnSprite->Initialise(*m_pStartBtnTexture))
	{
		LogManager::GetInstance().Log("Failed to initialise start button sprite");
		delete m_pStartBtnSprite;
		m_pStartBtnSprite = nullptr;

		delete m_pStartBtnTexture;
		m_pStartBtnTexture = nullptr;
		return false;
	}
	// Start position
	m_pStartBtnSprite->SetX(static_cast<int>(m_screenWidth / 2));
	m_pStartBtnSprite->SetY(static_cast<int>(m_screenHeight / 2));
	
	// Color change when been hovered
	m_pStartBtnSprite->SetRedTint(m_defaultRed);
	m_pStartBtnSprite->SetGreenTint(m_defaultGreen);
	m_pStartBtnSprite->SetBlueTint(m_defaultBlue);

	// Exit button
	m_pExitBtnTexture = new Texture();
	m_pExitBtnTexture->LoadTextTexture(sdlRenderer, "Exit Game", *m_pButtonFont, textColor);
	if (m_pExitBtnTexture->GetWidth() == 0)
	{
		LogManager::GetInstance().Log("Failed to load start game button");
		delete m_pExitBtnTexture;
		m_pExitBtnTexture = nullptr;
		return false;
	}

	m_pExitBtnSprite = new Sprite();
	if (!m_pExitBtnSprite->Initialise(*m_pExitBtnTexture))
	{
		LogManager::GetInstance().Log("Failed to initialise start button sprite");
		delete m_pExitBtnSprite;
		m_pExitBtnSprite = nullptr;

		delete m_pExitBtnTexture;
		m_pExitBtnTexture = nullptr;
		return false;
	}
	// Exit position
	m_pExitBtnSprite->SetX(static_cast<int>(m_screenWidth / 2));
	int startBtnHeight = m_pStartBtnTexture->GetHeight();
	m_pExitBtnSprite->SetY(m_pStartBtnSprite->GetY() + startBtnHeight + 30);

	// Color change when been hovered
	m_pExitBtnSprite->SetRedTint(m_defaultRed);
	m_pExitBtnSprite->SetGreenTint(m_defaultGreen);
	m_pExitBtnSprite->SetBlueTint(m_defaultBlue);


	return true;
}

void SceneTitlescreen::Process(float deltaTime, InputSystem& inputSystem)
{
	// Start button
	if (m_pStartBtnSprite)
	{
		m_bisMouseOverStart = IsMouseOverSprite(inputSystem, m_pStartBtnSprite);

		if (m_bisMouseOverStart)
		{
			m_pStartBtnSprite->SetRedTint(m_hoverRed);
			m_pStartBtnSprite->SetGreenTint(m_hoverGreen);
			m_pStartBtnSprite->SetBlueTint(m_hoverBlue);

			if (inputSystem.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED)
			{
				m_tSoundSystem.PlaySound("click");

				Game::GetInstance().SetCurrentScene(LOADING_INDEX);
			}
		}
		else
		{
			m_pStartBtnSprite->SetRedTint(m_defaultRed);
			m_pStartBtnSprite->SetGreenTint(m_defaultGreen);
			m_pStartBtnSprite->SetBlueTint(m_defaultBlue);
		}
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_RETURN) == BS_PRESSED ||
		(inputSystem.GetNumberOfControllersAttached() > 0 &&
		inputSystem.GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_START) == BS_PRESSED))
	{
		m_tSoundSystem.PlaySound("click");
		Game::GetInstance().SetCurrentScene(LOADING_INDEX);
	}


	// Exit button
	if (m_pExitBtnSprite)
	{
		m_bisMouseOverExit = IsMouseOverSprite(inputSystem, m_pExitBtnSprite);

		if (m_bisMouseOverExit)
		{
			m_pExitBtnSprite->SetRedTint(m_hoverRed);
			m_pExitBtnSprite->SetGreenTint(m_hoverGreen);
			m_pExitBtnSprite->SetBlueTint(m_hoverBlue);

			if (inputSystem.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED)
			{
				m_tSoundSystem.StopSound("bgm");

				Game::GetInstance().Quit();
			}
		}
		else
		{
			m_pExitBtnSprite->SetRedTint(m_defaultRed);
			m_pExitBtnSprite->SetGreenTint(m_defaultGreen);
			m_pExitBtnSprite->SetBlueTint(m_defaultBlue);
		}
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_ESCAPE) == BS_PRESSED ||
		(inputSystem.GetNumberOfControllersAttached() > 0 &&
		inputSystem.GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_BACK) == BS_PRESSED))
	{
		m_tSoundSystem.StopSound("bgm");

		Game::GetInstance().Quit();
	}

	if (m_pFMODSystem)
	{
		m_pFMODSystem->update();
	}

	m_tSoundSystem.Update();
}

void SceneTitlescreen::Draw(Renderer& renderer)
{
	renderer.SetCameraPosition(m_screenWidth / 2.0f, m_screenHeight / 2.0f);
	renderer.SetZoom(1.0f);

	if (m_pBackgroundSprite)
	{
		m_pBackgroundSprite->Draw(renderer);
	}

	if (m_pTitleSprite)
	{
		m_pTitleSprite->Draw(renderer);
	}

	if (m_pStartBtnSprite)
	{
		m_pStartBtnSprite->Draw(renderer);
	}

	if (m_pExitBtnSprite)
	{
		m_pExitBtnSprite->Draw(renderer);
	}
}

void SceneTitlescreen::DebugDraw()
{
	ImGui::Text("Scene: Title Screen");
}

bool SceneTitlescreen::IsMouseOverSprite(InputSystem& inputSystem, Sprite* sprite)
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