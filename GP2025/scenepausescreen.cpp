// THis include
#include "scenepausescreen.h"

// Local include
#include "renderer.h"
#include "texture.h"
#include "sprite.h"
#include "inputsystem.h"
#include "game.h"
#include "logmanager.h"
#include "font.h"
#include "vector2.h"
#include "xboxcontroller.h"

// Lib
#include <algorithm>
#include <iostream>

// Imgui
#include "imgui/imgui.h"

const int MAIN_SCENE_INDEX = 5;
const int TITLE_SCENE_INDEX = 3;

ScenePauseScreen::ScenePauseScreen()
	: m_pResumeBtnTexture(nullptr)
	, m_pResumeBtnSprite(nullptr)
	, m_pToTitleBtnTexture(nullptr)
	, m_pToTitleBtnSprite(nullptr)
	, m_pExitBtnTexture(nullptr)
	, m_pExitBtnSprite(nullptr)
	, m_pBackgroundSprite(nullptr)
	, m_pButtonFont(nullptr)
	, m_bisMouseOverResume(false)
	, m_bisMouseOverToTitle(false)
	, m_bisMouseOverExit(false)
	, m_screenWidth(0.0f)
	, m_screenHeight(0.0f)
{
}

ScenePauseScreen::~ScenePauseScreen()
{
	delete m_pResumeBtnSprite;
	m_pResumeBtnSprite = nullptr;
	delete m_pToTitleBtnSprite;
	m_pToTitleBtnSprite = nullptr;
	delete m_pExitBtnSprite;
	m_pExitBtnSprite = nullptr;

	delete m_pBackgroundSprite;
	m_pBackgroundSprite = nullptr;

	delete m_pResumeBtnTexture;
	m_pResumeBtnTexture = nullptr;
	delete m_pToTitleBtnTexture;
	m_pToTitleBtnTexture = nullptr;
	delete m_pExitBtnTexture;
	m_pExitBtnTexture = nullptr;

	delete m_pButtonFont;
	m_pButtonFont = nullptr;

	m_pSoundSystem.Release();
}

void ScenePauseScreen::OnEnter(bool reset)
{
	Game::GetInstance().m_pInputSystem->ShowMouseCursor(true);
}

void ScenePauseScreen::OnExit()
{
	Game::GetInstance().m_pInputSystem->ShowMouseCursor(false);
}

bool ScenePauseScreen::Initialise(Renderer& renderer)
{
	if (!m_pSoundSystem.Initialise())
	{
		LogManager::GetInstance().Log("Pause Scene SoundSystem failed to initialise!");
		return false;
	}
	m_pSoundSystem.LoadSound("click", "../assets/sound/Button_Click.mp3");

	m_screenWidth = static_cast<float>(renderer.GetWidth());
	m_screenHeight = static_cast<float>(renderer.GetHeight());

	SDL_Renderer* sdlRenderer = renderer.GetSDLRenderer();

	// Pause background
	m_pBackgroundSprite = renderer.CreateSprite("../assets/pauseBackground.png");
	m_pBackgroundSprite->SetX(static_cast<int>(m_screenWidth / 2));
	m_pBackgroundSprite->SetY(static_cast<int>(m_screenHeight / 2));
	float bgScaleX = m_screenWidth / m_pBackgroundSprite->GetWidth();
	float bgScaleY = m_screenHeight / m_pBackgroundSprite->GetHeight();
	m_pBackgroundSprite->SetScale(std::max(bgScaleX, bgScaleY));

	// Font
	const char* fontFilename = "../game/silkscreen.ttf";
	int buttonFontSize = 38;
	m_pButtonFont = new Font(fontFilename, buttonFontSize);
	SDL_Color textColor = { 255, 255, 255, 255 };

	// Resume Button
	m_pResumeBtnTexture = new Texture();
	m_pResumeBtnTexture->LoadTextTexture(sdlRenderer, "Resume Game", *m_pButtonFont, textColor);
	m_pResumeBtnSprite = new Sprite();
	m_pResumeBtnSprite->Initialise(*m_pResumeBtnTexture);
	m_pResumeBtnSprite->SetX(static_cast<int>(m_screenWidth / 2));
	m_pResumeBtnSprite->SetY(static_cast<int>(m_screenHeight / 2 - 60));
	m_pResumeBtnSprite->SetRedTint(m_defaultRed);
	m_pResumeBtnSprite->SetGreenTint(m_defaultGreen);
	m_pResumeBtnSprite->SetBlueTint(m_defaultBlue);

	// Quit to Main Menu Button
	m_pToTitleBtnTexture = new Texture();
	m_pToTitleBtnTexture->LoadTextTexture(sdlRenderer, "Quit to Main Menu", *m_pButtonFont, textColor);
	m_pToTitleBtnSprite = new Sprite();
	m_pToTitleBtnSprite->Initialise(*m_pToTitleBtnTexture);
	m_pToTitleBtnSprite->SetX(static_cast<int>(m_screenWidth / 2));
	int resumeBtnHeight = m_pResumeBtnTexture->GetHeight();
	m_pToTitleBtnSprite->SetY(m_pResumeBtnSprite->GetY() + resumeBtnHeight + 30);
	m_pToTitleBtnSprite->SetRedTint(m_defaultRed);
	m_pToTitleBtnSprite->SetGreenTint(m_defaultGreen);
	m_pToTitleBtnSprite->SetBlueTint(m_defaultBlue);

	// Exit Button
	m_pExitBtnTexture = new Texture();
	m_pExitBtnTexture->LoadTextTexture(sdlRenderer, "Exit Game", *m_pButtonFont, textColor);
	m_pExitBtnSprite = new Sprite();
	m_pExitBtnSprite->Initialise(*m_pExitBtnTexture);
	m_pExitBtnSprite->SetX(static_cast<int>(m_screenWidth / 2));
	int toTitleBtnHeight = m_pToTitleBtnTexture->GetHeight();
	m_pExitBtnSprite->SetY(m_pToTitleBtnSprite->GetY() + toTitleBtnHeight + 30);
	m_pExitBtnSprite->SetRedTint(m_defaultRed);
	m_pExitBtnSprite->SetGreenTint(m_defaultGreen);
	m_pExitBtnSprite->SetBlueTint(m_defaultBlue);

	return true;
}

void ScenePauseScreen::Process(float deltaTime, InputSystem& inputSystem)
{
	// Resume Game with keyboard/controller
	if (inputSystem.GetKeyState(SDL_SCANCODE_ESCAPE) == BS_PRESSED ||
		(inputSystem.GetNumberOfControllersAttached() > 0 &&
			(inputSystem.GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_START) == BS_PRESSED ||
				inputSystem.GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_BACK) == BS_PRESSED)))
	{
		m_pSoundSystem.PlaySound("click");
		Game::GetInstance().SetCurrentScene(MAIN_SCENE_INDEX);
		return;
	}

	// Resume button
	m_bisMouseOverResume = IsMouseOverSprite(inputSystem, m_pResumeBtnSprite);
	if (m_bisMouseOverResume)
	{
		m_pResumeBtnSprite->SetRedTint(m_hoverRed);
		m_pResumeBtnSprite->SetGreenTint(m_hoverGreen);
		m_pResumeBtnSprite->SetBlueTint(m_hoverBlue);
		if (inputSystem.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED)
		{
			m_pSoundSystem.PlaySound("click");
			Game::GetInstance().SetCurrentScene(MAIN_SCENE_INDEX);
		}
	}
	else
	{
		m_pResumeBtnSprite->SetRedTint(m_defaultRed);
		m_pResumeBtnSprite->SetGreenTint(m_defaultGreen);
		m_pResumeBtnSprite->SetBlueTint(m_defaultBlue);
	}

	// To Titlescreen button
	m_bisMouseOverToTitle = IsMouseOverSprite(inputSystem, m_pToTitleBtnSprite);
	if (m_bisMouseOverToTitle)
	{
		m_pToTitleBtnSprite->SetRedTint(m_hoverRed);
		m_pToTitleBtnSprite->SetGreenTint(m_hoverGreen);
		m_pToTitleBtnSprite->SetBlueTint(m_hoverBlue);
		if (inputSystem.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED)
		{
			m_pSoundSystem.PlaySound("click");
			Game::GetInstance().SetCurrentScene(TITLE_SCENE_INDEX);
		}
	}
	else
	{
		m_pToTitleBtnSprite->SetRedTint(m_defaultRed);
		m_pToTitleBtnSprite->SetGreenTint(m_defaultGreen);
		m_pToTitleBtnSprite->SetBlueTint(m_defaultBlue);
	}

	// Exit button
	m_bisMouseOverExit = IsMouseOverSprite(inputSystem, m_pExitBtnSprite);
	if (m_bisMouseOverExit)
	{
		m_pExitBtnSprite->SetRedTint(m_hoverRed);
		m_pExitBtnSprite->SetGreenTint(m_hoverGreen);
		m_pExitBtnSprite->SetBlueTint(m_hoverBlue);
		if (inputSystem.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED)
		{
			m_pSoundSystem.PlaySound("click");
			Game::GetInstance().Quit();
		}
	}
	else
	{
		m_pExitBtnSprite->SetRedTint(m_defaultRed);
		m_pExitBtnSprite->SetGreenTint(m_defaultGreen);
		m_pExitBtnSprite->SetBlueTint(m_defaultBlue);
	}

	m_pSoundSystem.Update();
}

void ScenePauseScreen::Draw(Renderer& renderer)
{
	renderer.SetCameraPosition(m_screenWidth / 2.0f, m_screenHeight / 2.0f);
	renderer.SetZoom(1.0f);

	if (m_pBackgroundSprite)
	{
		m_pBackgroundSprite->Draw(renderer);
	}
	if (m_pResumeBtnSprite)
	{
		m_pResumeBtnSprite->Draw(renderer);
	}
	if (m_pToTitleBtnSprite)
	{
		m_pToTitleBtnSprite->Draw(renderer);
	}
	if (m_pExitBtnSprite)
	{
		m_pExitBtnSprite->Draw(renderer);
	}
}

void ScenePauseScreen::DebugDraw()
{
	ImGui::Text("Scene: Pause Screen");
}

bool ScenePauseScreen::IsMouseOverSprite(InputSystem& inputSystem, Sprite* sprite)
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