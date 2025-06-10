#ifndef _ENDCUTSCENE_H__
#define _ENDCUTSCENE_H__

#include "scene.h"
#include <fmod.hpp>
#include <fmod_errors.h>
#include "soundsystem.h"
#include "vector2.h"

using namespace std;

class Renderer;
class InputSystem;
class Grid;
class CutscenePlayer;
class FallingRocks;

class Sprite;

class EndCutscene : public Scene {
public:
	EndCutscene();
	virtual ~EndCutscene();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

	void OnEnter() override;
	void OnExit() override;

	bool IsFinished();

protected:

private:
	EndCutscene(const EndCutscene& startCutScene);
	EndCutscene& operator=(const EndCutscene& startCutScene);

public:

protected:
	Grid* m_grid;
	CutscenePlayer* m_player;
	CutscenePlayer* m_trappedMiner;
	FallingRocks* m_rocks;

	Sprite* m_fade;

	Vector2 finishPos;

	bool m_sceneDone = false;
	bool m_rocksFallen = false;

	bool m_hitSound = false;
	bool m_hitSound2 = false;

	float m_startTimer = 0.0f;
	float m_startTime = 2.0f;

	float m_rockTimer = 0.0f;
	float m_rockTime = 3.5f;

	float m_reactionTimer = 0.0f;
	float m_reactionTime = 3.0f;

	float m_timer = 0.0f;
	float m_time = 5.0f;

	float alpha = 0.0f;

	SoundSystem m_tSoundSystem;

private:
};

#endif // !_ENDCUTSCENE_H__
