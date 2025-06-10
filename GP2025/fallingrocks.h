#ifndef _FALLINGROCKS_H__
#define _FALLINGROCKS_H__

#include "scene.h"

using namespace std;

class Renderer;
class InputSystem;
class Grid;
class CutscenePlayer;

class FallingRocks : public Scene {
public:
	FallingRocks();
	virtual ~FallingRocks();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

	bool GetStatus();

protected:

private:
	FallingRocks(const FallingRocks& fallingRocks);
	FallingRocks& operator=(const FallingRocks& fallingRocks);

public:

protected:
	Grid* m_grid;
	CutscenePlayer* m_player;

	bool m_sceneDone;

	float m_reactionTimer = 0.0f;
	float m_reactionTime = 5.0f;

private:
};

#endif // !_FALLINGROCKS_H__
