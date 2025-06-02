#include "gamestatemanager.h"

#include "Renderer.h"
#include "gamestate.h"
#include "inputsystem.h"

#include "splashautstate.h"
#include "splashfmodstate.h"
#include "scenemainstate.h"

#include "imgui/imgui.h"

#include "logmanager.h"

GameStateManager::GameStateManager(Renderer& renderer, InputSystem& inputSystem, FMOD::System* FMODSystem) : m_renderer(renderer), m_inputSystem(inputSystem)
, m_currentState(nullptr), m_gameLooping(true), m_debugging(false), m_FMODSystem(FMODSystem)
{
	m_states[GameStates::SPLASH_FMOD] = std::make_unique<SplashFMODState>();
	m_states[GameStates::SPLASH_AUT] = std::make_unique<SplashAUTState>();
	m_states[GameStates::MAIN] = std::make_unique<SceneMainState>(m_FMODSystem);

	ChangeState(GameStates::SPLASH_AUT);
}

void GameStateManager::ChangeState(GameStates newState)
{
	if (m_currentState) {
		m_currentState->Exit();
	}

	auto state = m_states.find(newState);

	if (state != m_states.end()) {
		m_currentState = state->second.get();
		m_currentState->SetRenderer(&m_renderer);
		m_currentState->SetInputSystem(&m_inputSystem);

		m_renderer.ClearTextures();
		m_currentState->Enter();
	}
}

void GameStateManager::Update(float deltatime)
{
	if (m_currentState) {
		m_currentState->Update(deltatime);
		m_gameLooping = m_currentState->GetGameStatus();

		GameStates nextState = m_currentState->GetNextState();
		if (nextState != GameStates::NONE) {
			ChangeState(nextState);
		}
	}

	if (m_inputSystem.GetKeyState(SDL_SCANCODE_INSERT) == BS_PRESSED) {
		m_debugging = !m_debugging;
	}

	if (m_inputSystem.GetKeyState(SDL_SCANCODE_ESCAPE) == BS_PRESSED) {
		m_gameLooping = !m_gameLooping;
	}
}

void GameStateManager::Draw()
{
	if (m_currentState) {
		m_currentState->Draw();
	}
}

void GameStateManager::DebugDraw()
{
	ImGui::Text("Current Scene: %s", GetCurrentScene().c_str());

	ImGui::Text("Current Frame Rate: %.2f", ImGui::GetIO().Framerate);

	ImGui::Separator();

	LogManager::GetInstance().DebugDraw();

	ImGui::Separator();

	if (m_currentState) {
		m_currentState->DebugDraw();
	}
}

bool GameStateManager::GetGameStatus()
{
	return m_gameLooping;
}

bool GameStateManager::GetDebuggingStatus()
{
	return m_debugging;
}

string GameStateManager::GetCurrentScene()
{
	if (!m_currentState) return "No State";

	if (dynamic_cast<SplashAUTState*>(m_currentState)) {
		return "Splash (AUT)";
	}
	else if (dynamic_cast<SplashFMODState*>(m_currentState)) {
		return "Splash (FMOD)";
	}

	return "Unknown State";
}
