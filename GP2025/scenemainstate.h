#ifndef _SCENEMAINSTATE_H__
#define _SCENEMAINSTATE_H__

#include "gamestate.h"
#include "fmod.hpp"

class SceneMain;

class SceneMainState : public GameState {
public:
	SceneMainState(FMOD::System* FMODSystem);

	void Enter() override;
	void Update(float deltatime) override;
	void Draw() override;
	void DebugDraw() override;
	void Exit() override;

	GameStates GetNextState() const override;

protected:

private:
	SceneMainState(const SceneMainState& sceneMainState);
	SceneMainState& operator=(const SceneMainState& sceneMainState);

public:

protected:
	SceneMain* m_scene;
	GameStates m_nextState;

	FMOD::System* m_FMODSystem;

private:
};

#endif // !_SCENEMAINSTATE_H__
