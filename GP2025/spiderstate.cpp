#include "spiderstate.h"

#include "spider.h"
#include "gameobjectpool.h"
#include "renderer.h"

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
		}
	}
}

void SpiderState::SetState(SpiderStates newState)
{
	if (m_currentState == newState) return;

	ExitState(m_currentState);
	EnterState(newState);
}

void SpiderState::Update(float deltaTime)
{
	if (m_active) {
		if (GameObject* obj = m_spiderPool->getObjectAtIndex(m_currentState)) {
			Spider* spider = dynamic_cast<Spider*>(obj);

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

void SpiderState::SetActive(bool active)
{
	m_active = active;
}

void SpiderState::EnterState(SpiderStates newState)
{
	if (GameObject* obj = m_spiderPool->getObjectAtIndex(newState)) {
		Spider* spider = dynamic_cast<Spider*>(obj);
		spider->SetAlive(false);
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
