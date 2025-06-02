#include "splashfmodstate.h"

#include "scenesplashfmod.h"

SplashFMODState::SplashFMODState() : m_scene(nullptr), m_nextState(GameStates::NONE) {}

void SplashFMODState::Enter()
{
	if (!m_renderer) return;

	m_scene = new SceneSplashFMOD();
	if (m_scene) m_scene->Initialise(*m_renderer);
}

void SplashFMODState::Update(float deltatime)
{
	if (m_scene && m_inputSystem) {
		m_scene->Process(deltatime, *m_inputSystem);
		if (m_scene->getStatus()) {
			m_nextState = GameStates::MAIN;
		}
	}
}

void SplashFMODState::Draw()
{
	if (m_scene && m_renderer) m_scene->Draw(*m_renderer);
}

void SplashFMODState::DebugDraw()
{
	if (m_scene) m_scene->DebugDraw();
}

void SplashFMODState::Exit()
{
	delete m_scene;
	m_scene = 0;
}

GameStates SplashFMODState::GetNextState() const
{
	return m_nextState;
}
