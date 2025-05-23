// COMP710 GP Framework 2022
// This include:
#include "scenecheckerboards.h"

// Local includes:
#include "renderer.h"
#include "sprite.h"
#include "imgui/imgui.h"
#include "inputsystem.h"
#include "game.h"
#include "logmanager.h"

// Library includes:
#include <cassert>
#include "fmod.hpp"

SceneCheckerboards::SceneCheckerboards(FMOD::System* pFMODSystem)
	: m_pFMODSystem(pFMODSystem)
	, m_pSound(0)
	, m_pChannel(0)
	, m_pCorners{ 0 }
	, m_pCentre(0)
	, m_angle(0.0f)
	, m_rotationSpeed(90.0f)
	, m_pAnimatedSprite(0)
{

}

SceneCheckerboards::~SceneCheckerboards()
{
	for (int k = 0; k < 4; ++k)
	{
		delete m_pCorners[k];
		m_pCorners[k] = 0;
	}

	delete m_pCentre;
	m_pCentre = 0;

	delete m_pAnimatedSprite;
	m_pAnimatedSprite = 0;
}

bool
SceneCheckerboards::Initialise(Renderer& renderer)
{
	m_pCentre = renderer.CreateSprite("../assets/board8x8.png");
	m_pCorners[0] = renderer.CreateSprite("../assets/board8x8.png");
	m_pCorners[1] = renderer.CreateSprite("../assets/board8x8.png");
	m_pCorners[2] = renderer.CreateSprite("../assets/board8x8.png");
	m_pCorners[3] = renderer.CreateSprite("../assets/board8x8.png");

	const int BOARD_HALF_WIDTH = m_pCentre->GetWidth() / 2;
	const int BOARD_HALF_HEIGHT = m_pCentre->GetHeight() / 2;
	const int SCREEN_WIDTH = renderer.GetWidth();
	const int SCREEN_HEIGHT = renderer.GetHeight();

	m_pCentre->SetX(SCREEN_WIDTH / 2);
	m_pCentre->SetY(SCREEN_HEIGHT / 2);

	// Top left white:
	m_pCorners[0]->SetX(BOARD_HALF_WIDTH);
	m_pCorners[0]->SetY(BOARD_HALF_HEIGHT);
	// Top right red:
	m_pCorners[1]->SetX(SCREEN_WIDTH - BOARD_HALF_WIDTH);
	m_pCorners[1]->SetY(BOARD_HALF_HEIGHT);
	m_pCorners[1]->SetGreenTint(0.0f);
	m_pCorners[1]->SetBlueTint(0.0f);
	// Bottom right green:
	m_pCorners[2]->SetX(SCREEN_WIDTH - BOARD_HALF_WIDTH);
	m_pCorners[2]->SetY(SCREEN_HEIGHT - BOARD_HALF_HEIGHT);
	m_pCorners[2]->SetRedTint(0.0f);
	m_pCorners[2]->SetBlueTint(0.0f);
	//Bottom left blue:
	m_pCorners[3]->SetX(BOARD_HALF_WIDTH);
	m_pCorners[3]->SetY(SCREEN_HEIGHT - BOARD_HALF_HEIGHT);
	m_pCorners[3]->SetRedTint(0.0f);


	//Animated Sprite
	m_pAnimatedSprite = renderer.CreateAnimatedSprite("../assets/anim8stripx2sheet.png");
	
	m_pAnimatedSprite->SetupFrames(64, 64);
	m_pAnimatedSprite->SetFrameDuration(0.1f);
	m_pAnimatedSprite->SetLooping(true);
	m_pAnimatedSprite->Animate();
	m_pAnimatedSprite->SetX(renderer.GetWidth() / 2);
	m_pAnimatedSprite->SetY(renderer.GetHeight() / 2);
	m_pAnimatedSprite->SetScale(3.0f);

	// Initialize FMOD sound
	FMOD_RESULT result = m_pFMODSystem->createSound("../assets/sound/boom.mp3", FMOD_DEFAULT, 0, &m_pSound);
	if (result != FMOD_OK) {
		LogManager::GetInstance().Log("FMOD sound loading failed!");
		return false;
	}

	return true;
}

void
SceneCheckerboards::Process(float deltaTime, InputSystem& inputSystem)
{
	for (int k = 0; k < 4; ++k)
	{
		m_pCorners[k]->Process(deltaTime);
	}
	m_angle += m_rotationSpeed * deltaTime;

	m_pCentre->SetAngle(m_angle);
	m_pCentre->Process(deltaTime);

	// Animated Sprite
	m_pAnimatedSprite->Process(deltaTime);

	// Checking for Left Click
	int mouseButtonState = inputSystem.GetMouseButtonState(SDL_BUTTON_LEFT);

	if (mouseButtonState == BS_PRESSED) {
		if (m_pFMODSystem) {
			m_pFMODSystem->playSound(m_pSound, 0, false, &m_pChannel);
		}
	}

	m_pFMODSystem->update(); // Keeping FMOD updated
}

void
SceneCheckerboards::Draw(Renderer& renderer)
{
	for (int k = 0; k < 4; ++k)
	{
		m_pCorners[k]->Draw(renderer);
	}

	m_pCentre->Draw(renderer);

	// Animated Sprite
	m_pAnimatedSprite->Draw(renderer);
}

void SceneCheckerboards::DebugDraw
()
{
	ImGui::Text("Scene: Checkerboards");
	
	ImGui::InputFloat("Rotation speed", &m_rotationSpeed);
	
	float scale = m_pCentre->GetScale();
	ImGui::SliderFloat("Demo scale", &scale, 0.0f, 2.0f, "%.3f");
	m_pCentre->SetScale(scale);
	
	int position[2];
	position[0] = m_pCentre->GetX();
	position[1] = m_pCentre->GetY();
	ImGui::InputInt2("Demo position", position);
	m_pCentre->SetX(position[0]);
	m_pCentre->SetY(position[1]);
	
	float tint[4];
	tint[0] = m_pCentre->GetRedTint();
	tint[1] = m_pCentre->GetGreenTint();
	tint[2] = m_pCentre->GetBlueTint();
	tint[3] = m_pCentre->GetAlpha();
	ImGui::ColorEdit4("Demo tint", tint);
	m_pCentre->SetRedTint(tint[0]);
	m_pCentre->SetGreenTint(tint[1]);
	m_pCentre->SetBlueTint(tint[2]);
	m_pCentre->SetAlpha(tint[3]);
}
