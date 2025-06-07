#ifndef __SPIDERSTATE_H__
#define __SPIDERSTATE_H__

enum SpiderStates {
	IDLE = 0,
	CRAWL,
	ATTACK = 2,
	HURT,
	DIE = 4
};

#include "gameobject.h"
#include "vector2.h"

class Spider;
class GameObjectPool;
class Renderer;

class SpiderState : public GameObject {
public:
	SpiderState();
	~SpiderState();

	void InitialiseSpiders(Renderer& renderer);
	void SetState(SpiderStates newState);
	void Update(float deltaTime, Vector2 playerPos);
	void Draw(Renderer& renderer);

	void UpdateAI(float deltaTime);

	int GetSpriteHeight();
	virtual int GetSpriteWidth() const override;
	virtual GameObject* Create() const override;
	virtual bool IsActive() const override;
	virtual void Reset() override;

	void SetActive(bool active);
	void SetPosition(Vector2 pos);

	Vector2 GetPosition();
	void ApplyPushback(Vector2 direction);

protected:
	void EnterState(SpiderStates newState);
	void ExitState(SpiderStates oldState);

	float Distance(Vector2 a, Vector2 b);
	Vector2 Normalise(Vector2 c);

	void Move(Vector2 direction, float deltaTime, Vector2 attackPos);

private:
	SpiderState(const SpiderState& spiderState);
	SpiderState& operator=(const SpiderState& spiderState);

public:

protected:
	GameObjectPool* m_spiderPool;
	SpiderStates m_currentState;
	Vector2 m_spiderPos;

	bool m_active;

	Vector2 m_target;
	float m_attackRange = 200.0f;

	float m_spiderWidth;
	float m_spiderHeight;

	float m_spiderHealth = 100.0f;

	int screenWidth;
	int screenHeight;

private:
};

#endif // !__SPIDERSTATE_H__
