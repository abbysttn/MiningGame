#include "scenesplashaut.h"

#include "Renderer.h"
#include "sprite.h"

#include <cassert>

#include "logmanager.h"

SceneSplashAUT::SceneSplashAUT() : m_pAUT(0), m_alpha(0.0f) {}

SceneSplashAUT::~SceneSplashAUT()
{
	delete m_pAUT;
	m_pAUT = 0;
}

bool SceneSplashAUT::Initialise(Renderer& renderer)
{
	m_pAUT = renderer.CreateSprite("..\\assets\\aut_logo.png");

	if (!m_pAUT) {
		LogManager::GetInstance().Log("Failed to load in start scene.");
	}

	const int halfSpriteW = m_pAUT->GetWidth() / 2;
	const int halfSpriteH = m_pAUT->GetHeight() / 2;

	const int screenW = renderer.GetWidth();
	const int screenH = renderer.GetHeight();

	m_pAUT->SetX(screenW / 2);
	m_pAUT->SetY(screenH / 2);

	return true;
}

void SceneSplashAUT::Process(float deltaTime, InputSystem& inputSystem)
{
	if (m_increase) {
		m_alpha += 0.3f * deltaTime;
		if (m_alpha >= 1.0f) {
			m_alpha = 1.0f;
			m_increase = false;
		}
	}
	else {
		m_alpha -= 0.3f * deltaTime;
		if (m_alpha <= 0.0f) {
			m_alpha = 0.0f;
			m_done = true;
		}
	}

	m_pAUT->SetAlpha(m_alpha);
	m_pAUT->Process(deltaTime);
}

void SceneSplashAUT::Draw(Renderer& renderer)
{
	renderer.SetClearColour(0, 0, 0);
	m_pAUT->Draw(renderer);
}

void SceneSplashAUT::DebugDraw()
{

}

bool SceneSplashAUT::getStatus()
{
	return m_done;
}
