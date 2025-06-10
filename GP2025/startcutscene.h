#ifndef _STARTCUTSCENE_H__
#define _STARTCUTSCENE_H__

#include "scene.h"

using namespace std;

class Renderer;
class InputSystem;
class Grid;
class Player;

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
	StartCutscene(const StartCutscene& flyingCutScene);
	StartCutscene& operator=(const StartCutscene& flyingCutScene);

public:

protected:
	Grid* m_grid;
	Player* m_player;

	bool m_sceneDone;

private:
};

#endif // !_STARTCUTSCENE_H__
