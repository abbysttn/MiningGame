// COMP710 GP Framework
#include <vector>
#include "fmod.hpp"

#ifndef GAME_H
#define GAME_H

// Forward declarations:
class Renderer;
class Sprite;
class Scene;
class InputSystem;

class Game
{
	// Member methods:
public:
	static Game& GetInstance();
	static void DestroyInstance();

	bool Initialise();
	bool DoGameLoop();
	void Quit();
	void InitBalls();
	void ToggleDebugWindow();

protected:
	void Process(float deltaTime);
	void Draw(Renderer& renderer);

	void ProcessFrameCounting(float deltaTime);

	void DebugDraw();

private:
	Game();
	~Game();
	Game(const Game& game);
	Game& operator=(const Game& game);

	// Member data:
public:

	// Ball struct
	struct Ball
	{
		Sprite* sprite;
		float x;
		float y;
		float vx;
		float vy;
		float scale;
		float r, g, b;
	};

	std::vector<Ball> m_balls;

	float bbWidth;
	float bbHeight;

protected:
	static Game* sm_pInstance;
	Renderer* m_pRenderer;
	Sprite* m_pCheckerboard;

	__int64 m_iLastTime;
	float m_fExecutionTime;
	float m_fElapsedSeconds;
	int m_iFrameCount;
	int m_iFPS;

	std::vector<Scene*> m_scenes;
	int m_iCurrentScene;

	InputSystem* m_pInputSystem;

	bool m_bShowDebugWindow;

#ifdef USE_LAG
	float m_fLag;
	int m_iUpdateCount;
#endif // USE_LAG

	bool m_bLooping;

	FMOD::System* m_pFMODSystem;
	FMOD::Sound* m_pSound;
	FMOD::Channel* m_pChannel;

private:
	
};

#endif // GAME_H