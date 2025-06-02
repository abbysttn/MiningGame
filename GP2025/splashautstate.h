#ifndef _SPLASHAUTSTATE_H__
#define _SPLASHAUTSTATE_H__

#include "gamestate.h"

class SceneSplashAUT;

class SplashAUTState : public GameState {
public:
	SplashAUTState();

	void Enter() override;
	void Update(float deltatime) override;
	void Draw() override;
	void DebugDraw() override;
	void Exit() override;

	GameStates GetNextState() const override;

protected:

private:
	SplashAUTState(const SplashAUTState& splashAUTState);
	SplashAUTState& operator=(const SplashAUTState& splashAUTState);

public:

protected:
	SceneSplashAUT* m_scene;
	GameStates m_nextState;

private:
};

#endif // !_SPLASHAUTSTATE_H__
