#ifndef __CUTSCENEPLAYER_H__
#define __CUTSCENEPLAYER_H__

#include "vector2.h"
#include "entity.h"

class Renderer;
class AnimatedSprite;

enum PlayerState {
	IDLE,
	MINE,
	JUMP
};

enum CutScene {
	START,
	ENDP1,
	ENDP2
};

class CutscenePlayer : public Entity {
public:
	CutscenePlayer();
	~CutscenePlayer();

	bool Initialise(Renderer& renderer);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);

	Vector2& Position();
	void SetScale(float scale);
	void SetFlip(bool flip);

	void SetState(PlayerState state);
	void SetCutscene(CutScene scene);



protected:

private:
	CutscenePlayer(const CutscenePlayer& cutscenePlayer);
	CutscenePlayer& operator=(const CutscenePlayer& cutscenePlayer);

public:

protected:
	Vector2 m_position;

	PlayerState m_animationState = IDLE;
	CutScene m_currentScene = START;

	AnimatedSprite* m_pIdleSprite = nullptr;
	AnimatedSprite* m_pMineSprite = nullptr;
	AnimatedSprite* m_pJumpSprite = nullptr;

	float m_scale = 40.0f;

	bool m_flip = false;

private:
};

#endif // !__CUTSCENEPLAYER_H__
