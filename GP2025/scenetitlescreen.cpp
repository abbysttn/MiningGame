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

// Lib includes
#include "imgui/imgui.h"
#include <algorithm>

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
{
}

SceneTitlescreen::~SceneTitlescreen()
{
	if (m_pBgmChannel)
	{
		m_pBgmChannel->stop();
		m_pBgmChannel = nullptr;
	}

	if (m_pClickSound)
	{
		m_pClickSound->release();
		m_pClickSound = nullptr;
	}

	if (m_pBgmSound)
	{
		m_pBgmSound->release();
		m_pBgmSound = nullptr;
	}

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
}

bool SceneTitlescreen::Initialise(Renderer& renderer)
{
	m_screenWidth = static_cast<float>(renderer.GetWidth());
	m_screenHeight = static_cast<float>(renderer.GetHeight());

	Game::GetInstance().m_pInputSystem->ShowMouseCursor(true);

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
	const char* fontName = "../game/silkscreen.ttf";
	int buttonFontSize = 38;

	// Buttonz
	m_pStartBtnTexture = new Texture();
	m_pStartBtnTexture->LoadTextTexture("Start Game", fontName, buttonFontSize);
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
	m_pExitBtnTexture->LoadTextTexture("Exit Game", fontName, buttonFontSize);
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

	if (m_pFMODSystem)
	{
		FMOD_RESULT result;

		// Button Sound
		result = m_pFMODSystem->createSound("../assets/sound/Button_Click.mp3", FMOD_DEFAULT, 0, &m_pClickSound);
		if (result != FMOD_OK)
		{
			LogManager::GetInstance().Log("Oops! Failed to load titlescreen button sound!");
		}

		// Title BGM
		result = m_pFMODSystem->createSound("../assets/sound/BGM_Titlescreen.mp3", FMOD_LOOP_NORMAL | FMOD_2D, 0, &m_pBgmSound);
		if (result != FMOD_OK)
		{
			LogManager::GetInstance().Log("Oops!! Failed to load Titlescreen BGM!!");
		}
		else
		{
			m_pFMODSystem->playSound(m_pBgmSound, nullptr, false, &m_pBgmChannel);
		}
	}

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
				if (m_pFMODSystem && m_pClickSound)
				{
					m_pFMODSystem->playSound(m_pClickSound, nullptr, false, nullptr);
				}

				if (m_pBgmChannel)
				{
					m_pBgmChannel->stop();

					m_pBgmChannel = nullptr;
				}

				Game::GetInstance().SetCurrentScene(1);
			}
		}
		else
		{
			m_pStartBtnSprite->SetRedTint(m_defaultRed);
			m_pStartBtnSprite->SetGreenTint(m_defaultGreen);
			m_pStartBtnSprite->SetBlueTint(m_defaultBlue);
		}
	}

	// Exit button
	if (m_pExitBtnSprite)
	{
		m_bisMouseOverStart = IsMouseOverSprite(inputSystem, m_pExitBtnSprite);

		if (m_bisMouseOverStart)
		{
			m_pExitBtnSprite->SetRedTint(m_hoverRed);
			m_pExitBtnSprite->SetGreenTint(m_hoverGreen);
			m_pExitBtnSprite->SetBlueTint(m_hoverBlue);

			if (inputSystem.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED)
			{
				if (m_pFMODSystem && m_pClickSound)
				{
					m_pFMODSystem->playSound(m_pClickSound, nullptr, false, nullptr);
				}

				if (m_pBgmChannel)
				{
					m_pBgmChannel->stop();

					m_pBgmChannel = nullptr;
				}

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

	if (m_pFMODSystem)
	{
		m_pFMODSystem->update();
	}
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