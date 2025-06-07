#include "spiderstate.h"

#include "spider.h"
#include "gameobjectpool.h"
#include "renderer.h"

#include "collisionhelper.h"
#include "logmanager.h"
#include "gridstate.h"
#include <algorithm>

#include <string>

SpiderState::SpiderState() : m_currentState(SpiderStates::IDLE), m_spiderPool(nullptr) {}

SpiderState::~SpiderState()
{
	delete m_spiderPool;
	m_spiderPool = nullptr;
}

void SpiderState::InitialiseSpiders(Renderer& renderer, int screenWidth, int screenHeight)
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

	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
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
		if (m_spiderHealth <= 0.0f) {
			SetState(DIE);
		}

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
	Vector2 attackPos = spider->Attack(direction, deltaTime);

	switch (m_currentState) {
	case 0:
		if (distanceToTarget < 500.0f) {
			SetState(CRAWL);
		}
		break;

	case 1:
		Move(direction, deltaTime, attackPos);

		if (distanceToTarget > 500.0f) {
			SetState(IDLE);
		}

		if (distanceToTarget < m_attackRange) {
			SetState(ATTACK);
		}
		break;

	case 2:
		Move(direction, deltaTime, attackPos);

		if (distanceToTarget > m_attackRange) {
			SetState(CRAWL);
		}
		break;

	case 3:
		if (!spider->IsPushed()) {
			m_spiderHealth -= 50.0f;
			SetState(CRAWL);
		}
		break;

	case 4:
		if (spider->IsDead()) {
			spider->SetAlive(false);
			m_active = false;
		}
	}

	const int spriteHalfWidth = GetSpriteWidth() / 2;

	float wallMarginX = screenWidth * 0.00f;

	float minX = wallMarginX + spriteHalfWidth;
	float maxX = screenWidth - wallMarginX - spriteHalfWidth;

	m_spiderPos.x = std::max(minX, std::min(maxX, m_spiderPos.x));
}

int SpiderState::GetSpriteHeight()
{
	int height = 0;
	if (GameObject* obj = m_spiderPool->getObjectAtIndex(m_currentState)) {
		Spider* spider = dynamic_cast<Spider*>(obj);
		height = spider->GetSpriteHeight();
	}

	return height;
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

Vector2 SpiderState::GetPosition()
{
	return m_spiderPos;
}

void SpiderState::ApplyPushback(Vector2 direction)
{
	if (GameObject* obj = m_spiderPool->getObjectAtIndex(m_currentState)) {
		Spider* spider = dynamic_cast<Spider*>(obj);
		spider->ApplyPushback(direction);
	}
}

SpiderStates SpiderState::GetState()
{
	return m_currentState;
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

void SpiderState::Move(Vector2 direction, float deltaTime, Vector2 attackPos)
{
	Vector2 testPos = m_spiderPos;

	testPos.x += direction.x * 50.0f * deltaTime;

	if (m_currentState == SpiderStates::ATTACK) {
		testPos = attackPos;
	}

	float paddingX = (m_spiderWidth / 2.0f) + 5.0f;
	float paddingY = (m_spiderHeight / 2.0f) + 20.0f;

	Box testBoxX(testPos.x + paddingX, m_spiderPos.y + paddingY, m_spiderWidth, m_spiderHeight);
	bool canMoveX = !GridState::GetInstance().CheckCollision(testBoxX);

	m_climbing = false;

	if (canMoveX) {
		m_spiderPos.x = testPos.x;
	}
	else {
		m_spiderPos.y -= 25.0f * deltaTime;
		m_climbing = true;
	}

	if (!m_climbing) {
		float finalGroundY = FindGround(m_spiderPos.x);

		if (abs(m_spiderPos.y - finalGroundY) > 1.0f) {
			float adjustSpeed = 150.0f * deltaTime;

			if (m_spiderPos.y < finalGroundY) {
				m_spiderPos.y = std::min(m_spiderPos.y + adjustSpeed, finalGroundY);
			}
			else {
				m_spiderPos.y = std::max(m_spiderPos.y - adjustSpeed, finalGroundY);
			}
		}
		else {
			m_spiderPos.y = finalGroundY;
		}
	}

	Box finalPos(m_spiderPos.x + paddingX, m_spiderPos.y + paddingY, m_spiderWidth, m_spiderHealth);

	if (GridState::GetInstance().CheckCollision(finalPos)) {
		m_spiderPos.y -= 1.0f;
	}
}

float SpiderState::FindGround(float x)
{
	float paddingX = (m_spiderWidth / 2.0f) + 5.0f;
	float paddingY = (m_spiderHeight / 2.0f) + 20.0f;

	float testY = m_spiderPos.y;
	float lowestValidY = testY;

	Box testBox(x + paddingX, testY + paddingY, m_spiderWidth, m_spiderHeight);

	while (GridState::GetInstance().CheckCollision(testBox) && testY > 0) {
		testY -= 1.0f;
		testBox.y = testY + paddingY;
		lowestValidY = testY;
	}

	testBox.y = testY + paddingY;
	while (!GridState::GetInstance().CheckCollision(testBox) && testY < screenHeight) {
		lowestValidY = testY;
		testY += 1.0f;
		testBox.y = testY + paddingY;
	}

	return lowestValidY;
}
