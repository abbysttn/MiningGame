#include "spiderstate.h"

#include "spider.h"
#include "gameobjectpool.h"
#include "renderer.h"

#include "collisionhelper.h"
#include "gridstate.h"

#include <string>

SpiderState::SpiderState() : m_currentState(SpiderStates::IDLE), m_spiderPool(nullptr) {}

SpiderState::~SpiderState()
{
	delete m_spiderPool;
	m_spiderPool = nullptr;
}

void SpiderState::InitialiseSpiders(Renderer& renderer)
{
	if (m_spiderPool) return;

	m_spiderPool = new GameObjectPool(Spider(), 5);

	for (size_t i = 0; i < m_spiderPool->totalCount(); i++) {
		if (GameObject* obj = m_spiderPool->getObjectAtIndex(i)) {
			Spider* spider = dynamic_cast<Spider*>(obj);

			string filepath = "../assets/spider_";

			switch (i) {
			case 0:
				filepath += "idle_r.png";
				break;
			case 1:
				filepath += "walk_r.png";
				break;
			case 2:
				filepath += "attack_r.png";
				break;
			case 3:
				filepath += "hurt_r.png";
				break;
			case 4:
				filepath += "die_r.png";
				break;
			default:
				filepath += "idle_r.png";
				break;
			}

			spider->Initialise(renderer, filepath.c_str());

			if (i == m_currentState) {
				spider->SetAlive(true);
			}

			m_spiderWidth = (float)spider->GetSpriteWidth();
			m_spiderHeight = (float)spider->GetSpriteHeight();
		}
	}
}

void SpiderState::SetState(SpiderStates newState)
{
	if (m_currentState == newState) return;

	ExitState(m_currentState);
	EnterState(newState);
}

void SpiderState::Update(float deltaTime, Vector2 playerPos)
{
	if (m_active) {
		m_target = playerPos;
		UpdateAI(deltaTime);

		if (GameObject* obj = m_spiderPool->getObjectAtIndex(m_currentState)) {
			Spider* spider = dynamic_cast<Spider*>(obj);
			spider->Position() = m_spiderPos;
			spider->Process(deltaTime);
		}
	}
}

void SpiderState::Draw(Renderer& renderer)
{
	if (GameObject* obj = m_spiderPool->getObjectAtIndex(m_currentState)) {
		Spider* spider = dynamic_cast<Spider*>(obj);
		spider->Draw(renderer);
	}
}

void SpiderState::UpdateAI(float deltaTime)
{
	if (!m_spiderPool) return;

	Spider* spider = dynamic_cast<Spider*>(m_spiderPool->getObjectAtIndex(m_currentState));
	if (!spider) return;

	m_spiderPos = spider->Position();
	float distanceToTarget = Distance(m_target, m_spiderPos);
	Vector2 direction = Normalise(m_target - m_spiderPos);

	switch (m_currentState) {
	case 0:
		if (distanceToTarget < 200.0f) {
			SetState(CRAWL);
		}
		break;

	case 1:
		Move(direction, deltaTime);

		if (distanceToTarget > 200.0f) {
			SetState(IDLE);
		}

		if (distanceToTarget < m_attackRange) {
			SetState(ATTACK);
		}
		break;

	case 2:
		if (distanceToTarget > m_attackRange) {
			SetState(CRAWL);
		}
		break;
	}
}

int SpiderState::GetSpriteWidth() const
{
	int width = 0;
	if (GameObject* obj = m_spiderPool->getObjectAtIndex(m_currentState)) {
		Spider* spider = dynamic_cast<Spider*>(obj);
		width = spider->GetSpriteWidth();
	}

	return width;
}

GameObject* SpiderState::Create() const
{
	return new SpiderState();
}

bool SpiderState::IsActive() const
{
	return m_active;
}

void SpiderState::Reset()
{
	m_currentState = SpiderStates::IDLE;
}

void SpiderState::SetActive(bool active)
{
	m_active = active;
}

void SpiderState::SetPosition(Vector2 pos)
{
	if (GameObject* obj = m_spiderPool->getObjectAtIndex(m_currentState)) {
		Spider* spider = dynamic_cast<Spider*>(obj);
		spider->Position() = pos;
	}
}

void SpiderState::EnterState(SpiderStates newState)
{
	if (GameObject* obj = m_spiderPool->getObjectAtIndex(newState)) {
		Spider* spider = dynamic_cast<Spider*>(obj);
		spider->SetAlive(true);
		spider->Position() = m_spiderPos;
	}

	m_currentState = newState;
}

void SpiderState::ExitState(SpiderStates oldState)
{
	if (GameObject* obj = m_spiderPool->getObjectAtIndex(oldState)) {
		Spider* spider = dynamic_cast<Spider*>(obj);
		spider->SetAlive(false);
	}
}

float SpiderState::Distance(Vector2 a, Vector2 b)
{
	float dx = b.x - a.x;
	float dy = b.y - a.y;

	return sqrtf(dx * dx + dy * dy);
}

Vector2 SpiderState::Normalise(Vector2 c)
{
	float length = sqrtf(c.x * c.x + c.y * c.y);
	
	if (length > 0) {
		return Vector2(c.x / length, c.y / length);
	}

	return Vector2(0, 0);
}

void SpiderState::Move(Vector2 direction, float deltaTime)
{
	Vector2 testPos = m_spiderPos;

	testPos += direction * 50.0f * deltaTime;

	float paddingX = (m_spiderWidth / 2.0f) + 5.0f;
	float paddingY = (m_spiderHeight / 2.0f) + 20.0f;

	Box testBoxX(testPos.x + paddingX, m_spiderPos.y + paddingY, m_spiderWidth, m_spiderHeight);

	if (!GridState::GetInstance().CheckCollision(testBoxX)) {
		m_spiderPos.x = testPos.x;
	}

	Box testBoxY(m_spiderPos.x + paddingX, testPos.y + paddingY, m_spiderWidth, m_spiderHeight);

	if (!GridState::GetInstance().CheckCollision(testBoxY)) {
		m_spiderPos.y = testPos.y;
	}
}
