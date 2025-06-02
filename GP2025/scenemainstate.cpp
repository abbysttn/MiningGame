#include "scenemainstate.h"

#include "scenemain.h"

SceneMainState::SceneMainState(FMOD::System* FMODSystem) : m_scene(nullptr), m_nextState(GameStates::NONE), m_FMODSystem(FMODSystem) {}

void SceneMainState::Enter()
{
	if (!m_renderer) return;

	m_scene = new SceneMain(m_FMODSystem);
	if (m_scene) m_scene->Initialise(*m_renderer);
}

void SceneMainState::Update(float deltatime)
{
	if (m_scene && m_inputSystem) {
		m_scene->Process(deltatime, *m_inputSystem);
	}
}

void SceneMainState::Draw()
{
	if (m_scene && m_renderer) m_scene->Draw(*m_renderer);
}

void SceneMainState::DebugDraw()
{
	if (m_scene) m_scene->DebugDraw();
}

void SceneMainState::Exit()
{
	delete m_scene;
	m_scene = 0;
}

GameStates SceneMainState::GetNextState() const
{
	return m_nextState;
}
