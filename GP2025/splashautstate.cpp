#include "splashautstate.h"

#include "scenesplashaut.h"

SplashAUTState::SplashAUTState() : m_scene(nullptr), m_nextState(GameStates::NONE) {}

void SplashAUTState::Enter()
{
	if (!m_renderer) return;

	m_scene = new SceneSplashAUT();
	if (m_scene) m_scene->Initialise(*m_renderer);
}

void SplashAUTState::Update(float deltatime)
{
	if (m_scene && m_inputSystem) {
		m_scene->Process(deltatime, *m_inputSystem);
		if (m_scene->getStatus()) {
			m_nextState = GameStates::SPLASH_FMOD;
		}
	}
}

void SplashAUTState::Draw()
{
	if (m_scene && m_renderer) m_scene->Draw(*m_renderer);
}

void SplashAUTState::DebugDraw()
{
	if (m_scene) m_scene->DebugDraw();
}

void SplashAUTState::Exit()
{
	delete m_scene;
	m_scene = 0;
}

GameStates SplashAUTState::GetNextState() const
{
	return m_nextState;
}
