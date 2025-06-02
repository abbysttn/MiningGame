#ifndef _GAMESTATEMANAGER_H__
#define _GAMESTATEMANAGER_H__

#include <unordered_map>
#include <memory>
#include <string>
#include "fmod.hpp"

using namespace std;

class Renderer;
class InputSystem;
enum GameStates;
enum GameDifficulty;
class GameState;

class GameStateManager {
public:
	GameStateManager(Renderer& renderer, InputSystem& inputSystem, FMOD::System* FMODSystem);

	void ChangeState(GameStates newState);
	void Update(float deltatime);
	void Draw();
	void DebugDraw();

	bool GetGameStatus();

	bool GetDebuggingStatus();

protected:
	string GetCurrentScene();

private:
	GameStateManager(const GameStateManager& gameStateManager);
	GameStateManager& operator=(const GameStateManager& gameStateManager);

public:

protected:
	std::unordered_map<GameStates, std::unique_ptr<GameState>> m_states;
	GameState* m_currentState;
	GameDifficulty m_currentDifficulty;
	Renderer& m_renderer;
	InputSystem& m_inputSystem;
	FMOD::System* m_FMODSystem;

	bool m_wonGame;

	bool m_gameLooping;

	bool m_debugging;

private:
};

#endif // !_GAMESTATEMANAGER_H__
