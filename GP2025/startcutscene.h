#ifndef _STARTCUTSCENE_H__
#define _STARTCUTSCENE_H__

#include "scene.h"
#include <fmod.hpp>
#include <fmod_errors.h>
#include "soundsystem.h"

using namespace std;

class Renderer;
class InputSystem;
class Grid;
class CutscenePlayer;
class FallingRocks;

class Sprite;

class StartCutscene : public Scene {
public:
	StartCutscene();
	virtual ~StartCutscene();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

	void OnEnter(bool reset) override;
	void OnExit() override;

	bool IsFinished();

protected:

private:
	StartCutscene(const StartCutscene& startCutScene);
	StartCutscene& operator=(const StartCutscene& startCutScene);

public:

protected:
	Grid* m_grid;
	CutscenePlayer* m_player;
	FallingRocks* m_rocks;

	Sprite* m_fade;

	bool m_sceneDone = false;

	float m_startTimer = 0.0f;
	float m_startTime = 2.0f;

	bool m_hitSound = false;
	bool m_hitSound2 = false;

	float m_reactionTimer = 0.0f;
	float m_reactionTime = 5.0f;

	float m_timer = 0.0f;
	float m_time = 2.0f;

	float alpha = 0.0f;

	SoundSystem m_tSoundSystem;

private:
};

#endif // !_STARTCUTSCENE_H__
