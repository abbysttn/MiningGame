#ifndef _STARTCUTSCENE_H__
#define _STARTCUTSCENE_H__

#include "scene.h"

using namespace std;

class Renderer;
class InputSystem;
class Grid;
class CutscenePlayer;
class FallingRocks;

class StartCutscene : public Scene {
public:
	StartCutscene();
	virtual ~StartCutscene();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

	bool GetStatus();

protected:

private:
	StartCutscene(const StartCutscene& startCutScene);
	StartCutscene& operator=(const StartCutscene& startCutScene);

public:

protected:
	Grid* m_grid;
	CutscenePlayer* m_player;
	FallingRocks* m_rocks;

	bool m_sceneDone;

	float m_reactionTimer = 0.0f;
	float m_reactionTime = 5.0f;

private:
};

#endif // !_STARTCUTSCENE_H__
