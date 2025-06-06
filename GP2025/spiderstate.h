#ifndef __SPIDERSTATE_H__
#define __SPIDERSTATE_H__

enum SpiderStates {
	IDLE = 0,
	CRAWL,
	ATTACK = 2,
	HURT,
	DIE = 4
};

class Spider;
class GameObjectPool;
class Renderer;

class SpiderState {
public:
	SpiderState();
	~SpiderState();

	void InitialiseSpiders(Renderer& renderer);
	void SetState(SpiderStates newState);
	void Update(float deltaTime);
	void Draw(Renderer& renderer);

	void SetActive(bool active);

protected:
	void EnterState(SpiderStates newState);
	void ExitState(SpiderStates oldState);

private:
	SpiderState(const SpiderState& spiderState);
	SpiderState& operator=(const SpiderState& spiderState);

public:

protected:
	GameObjectPool* m_spiderPool;
	SpiderStates m_currentState;

	bool m_active;

private:
};

#endif // !__SPIDERSTATE_H__
