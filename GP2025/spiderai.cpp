#include "spiderai.h"

SpiderAI::SpiderAI(Spider& spider, SpiderStates currentState) : m_spider(spider), m_currentState(currentState) {}

void SpiderAI::Update(float deltaTime)
{
	switch (m_currentState) {
	case 0:
		HandleIdle();
		break;
	case 1:
		HandleMovement();
		break;
	case 2:
		HandleAttack();
		break;
	}
}

void SpiderAI::SetTarget(const Vector2& target)
{
	m_target = target;
}

void SpiderAI::HandleIdle()
{
	float distanceX = m_spider.Position().x - m_target.x;
	float distanceY = m_spider.Position().y - m_target.y;

	if (distanceX <= m_attackRange || distanceY <= m_attackRange) {
		
	}
}

void SpiderAI::HandleMovement()
{
}

void SpiderAI::HandleAttack()
{
}
