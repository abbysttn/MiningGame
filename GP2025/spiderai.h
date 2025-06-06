#ifndef __SPIDERAI_H__
#define __SPIDERAI_H__

#include "spider.h"
#include "vector2.h"
#include "spiderstate.h"

class SpiderAI {
public:
	SpiderAI(Spider& spider, SpiderStates currentState);

	void Update(float deltaTime);
	void SetTarget(const Vector2& target);

protected:
	void HandleIdle();
	void HandleMovement();
	void HandleAttack();

private:
	SpiderAI(const SpiderAI& spiderAI);
	SpiderAI& operator=(const SpiderAI& spiderAI);

public:

protected:
	Spider& m_spider;
	SpiderStates m_currentState;
	Vector2 m_target;
	float m_attackRange = 100.0f;

private:
};

#endif // !__SPIDERAI_H__
