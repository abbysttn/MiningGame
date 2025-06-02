#ifndef _SPLASHFMODSTATE_H__
#define _SPLASHFMODSTATE_H__

#include "gamestate.h"

class SceneSplashFMOD;

class SplashFMODState : public GameState {
public:
	SplashFMODState();

	void Enter() override;
	void Update(float deltatime) override;
	void Draw() override;
	void DebugDraw() override;
	void Exit() override;

	GameStates GetNextState() const override;

protected:

private:
	SplashFMODState(const SplashFMODState& splashFMODState);
	SplashFMODState& operator=(const SplashFMODState& splashFMODState);

public:

protected:
	SceneSplashFMOD* m_scene;
	GameStates m_nextState;

private:
};

#endif // !_SPLASHFMODSTATE_H__
