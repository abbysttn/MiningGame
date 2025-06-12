// This include
#include "scenecontrolsmenu.h"

// Local include
#include "renderer.h"
#include "texture.h"
#include "sprite.h"
#include "inputsystem.h"
#include "game.h"
#include "logmanager.h"
#include "font.h"
#include "vector2.h"

// Imgui
#include "imgui/imgui.h"

// Scene index
const int PAUSE_SCENE_INDEX = 7;
const int KEYBOARD_CONTROLS_SCENE_INDEX = 9;
const int CONTROLLER_CONTROLS_SCENE_INDEX = 10;

SceneControlsMenu::SceneControlsMenu()
	: m_pKeyboardBtnTexture(nullptr), m_pKeyboardBtnSprite(nullptr)
	, m_pControllerBtnTexture(nullptr), m_pControllerBtnSprite(nullptr)
	, m_pBackBtnTexture(nullptr), m_pBackBtnSprite(nullptr)
	, m_pBackgroundSprite(nullptr), m_pButtonFont(nullptr)
	, m_bisMouseOverKeyboard(false), m_bisMouseOverController(false), m_bisMouseOverBack(false)
	, m_screenWidth(0.0f), m_screenHeight(0.0f)
{
}

SceneControlsMenu::~SceneControlsMenu()
{
	delete m_pKeyboardBtnSprite; m_pKeyboardBtnSprite = nullptr;
	delete m_pControllerBtnSprite; m_pControllerBtnSprite = nullptr;
	delete m_pBackBtnSprite; m_pBackBtnSprite = nullptr;
	delete m_pBackgroundSprite; m_pBackgroundSprite = nullptr;

	delete m_pKeyboardBtnTexture; m_pKeyboardBtnTexture = nullptr;
	delete m_pControllerBtnTexture; m_pControllerBtnTexture = nullptr;
	delete m_pBackBtnTexture; m_pBackBtnTexture = nullptr;

	delete m_pButtonFont; m_pButtonFont = nullptr;

	m_pSoundSystem.Release();
}

bool SceneControlsMenu::Initialise(Renderer& renderer)
{
	if (!m_pSoundSystem.Initialise())
	{
		LogManager::GetInstance().Log("Controls Menu SoundSystem failed to initialise!");
		return false;
	}
	m_pSoundSystem.LoadSound("click", "../assets/sound/Button_Click.mp3");

	m_screenWidth = static_cast<float>(renderer.GetWidth());
	m_screenHeight = static_cast<float>(renderer.GetHeight());
	SDL_Renderer* sdlRenderer = renderer.GetSDLRenderer();

	// background
	m_pBackgroundSprite = renderer.CreateSprite("../assets/pauseBackground.png");
	m_pBackgroundSprite->SetX(static_cast<int>(m_screenWidth / 2));
	m_pBackgroundSprite->SetY(static_cast<int>(m_screenHeight / 2));
	float bgScaleX = m_screenWidth / m_pBackgroundSprite->GetWidth();
	float bgScaleY = m_screenHeight / m_pBackgroundSprite->GetHeight();
	m_pBackgroundSprite->SetScale(std::max(bgScaleX, bgScaleY));

	// Font
	const char* fontFilename = "../assets/fonts/joystix.otf";
	int buttonFontSize = 38;
	m_pButtonFont = new Font(fontFilename, buttonFontSize);
	SDL_Color textColor = { 255, 255, 255, 255 };

	int padding = 50;
	int buttonSpacing = 80;

	// Keyboard Controls Button
	m_pKeyboardBtnTexture = new Texture();
	m_pKeyboardBtnTexture->LoadTextTexture(sdlRenderer, "Keyboard Controls", *m_pButtonFont, textColor);
	m_pKeyboardBtnSprite = new Sprite();
	m_pKeyboardBtnSprite->Initialise(*m_pKeyboardBtnTexture);
	m_pKeyboardBtnSprite->SetX(m_pKeyboardBtnTexture->GetWidth() / 2 + padding);
	m_pKeyboardBtnSprite->SetY(static_cast<int>(m_screenHeight / 2 - buttonSpacing));

	// Controller Controls Button
	m_pControllerBtnTexture = new Texture();
	m_pControllerBtnTexture->LoadTextTexture(sdlRenderer, "Controller Controls", *m_pButtonFont, textColor);
	m_pControllerBtnSprite = new Sprite();
	m_pControllerBtnSprite->Initialise(*m_pControllerBtnTexture);
	m_pControllerBtnSprite->SetX(m_pControllerBtnTexture->GetWidth() / 2 + padding);
	m_pControllerBtnSprite->SetY(static_cast<int>(m_screenHeight / 2));

	// Back Button
	m_pBackBtnTexture = new Texture();
	m_pBackBtnTexture->LoadTextTexture(sdlRenderer, "Back", *m_pButtonFont, textColor);
	m_pBackBtnSprite = new Sprite();
	m_pBackBtnSprite->Initialise(*m_pBackBtnTexture);
	m_pBackBtnSprite->SetX(m_pBackBtnTexture->GetWidth() / 2 + padding);
	m_pBackBtnSprite->SetY(static_cast<int>(m_screenHeight / 2 + buttonSpacing));

	return true;
}

void SceneControlsMenu::Process(float deltaTime, InputSystem& inputSystem)
{
	// Keyboard button logic
	m_bisMouseOverKeyboard = IsMouseOverSprite(inputSystem, m_pKeyboardBtnSprite);
	if (m_bisMouseOverKeyboard)
	{
		m_pKeyboardBtnSprite->SetRedTint(m_hoverRed); m_pKeyboardBtnSprite->SetGreenTint(m_hoverGreen); m_pKeyboardBtnSprite->SetBlueTint(m_hoverBlue);
		if (inputSystem.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED)
		{
			m_pSoundSystem.PlaySound("click");
			Game::GetInstance().SetCurrentScene(KEYBOARD_CONTROLS_SCENE_INDEX);
		}
	}
	else
	{
		m_pKeyboardBtnSprite->SetRedTint(m_defaultRed); m_pKeyboardBtnSprite->SetGreenTint(m_defaultGreen); m_pKeyboardBtnSprite->SetBlueTint(m_defaultBlue);
	}

	// Controller button logic
	m_bisMouseOverController = IsMouseOverSprite(inputSystem, m_pControllerBtnSprite);
	if (m_bisMouseOverController)
	{
		m_pControllerBtnSprite->SetRedTint(m_hoverRed); m_pControllerBtnSprite->SetGreenTint(m_hoverGreen); m_pControllerBtnSprite->SetBlueTint(m_hoverBlue);
		if (inputSystem.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED)
		{
			m_pSoundSystem.PlaySound("click");
			Game::GetInstance().SetCurrentScene(CONTROLLER_CONTROLS_SCENE_INDEX);
		}
	}
	else
	{
		m_pControllerBtnSprite->SetRedTint(m_defaultRed); m_pControllerBtnSprite->SetGreenTint(m_defaultGreen); m_pControllerBtnSprite->SetBlueTint(m_defaultBlue);
	}

	// Back button logic
	m_bisMouseOverBack = IsMouseOverSprite(inputSystem, m_pBackBtnSprite);
	if (m_bisMouseOverBack)
	{
		m_pBackBtnSprite->SetRedTint(m_hoverRed); m_pBackBtnSprite->SetGreenTint(m_hoverGreen); m_pBackBtnSprite->SetBlueTint(m_hoverBlue);
		if (inputSystem.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED)
		{
			m_pSoundSystem.PlaySound("click");
			Game::GetInstance().SetCurrentScene(PAUSE_SCENE_INDEX);
		}
	}
	else
	{
		m_pBackBtnSprite->SetRedTint(m_defaultRed); m_pBackBtnSprite->SetGreenTint(m_defaultGreen); m_pBackBtnSprite->SetBlueTint(m_defaultBlue);
	}

	m_pSoundSystem.Update();
}

void SceneControlsMenu::Draw(Renderer& renderer)
{
	renderer.SetCameraPosition(m_screenWidth / 2.0f, m_screenHeight / 2.0f);
	renderer.SetZoom(1.0f);

	if (m_pBackgroundSprite) m_pBackgroundSprite->Draw(renderer);
	if (m_pKeyboardBtnSprite) m_pKeyboardBtnSprite->Draw(renderer);
	if (m_pControllerBtnSprite) m_pControllerBtnSprite->Draw(renderer);
	if (m_pBackBtnSprite) m_pBackBtnSprite->Draw(renderer);
}

void SceneControlsMenu::DebugDraw()
{
	ImGui::Text("Scene: Controls Menu");
}

bool SceneControlsMenu::IsMouseOverSprite(InputSystem& inputSystem, Sprite* sprite)
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