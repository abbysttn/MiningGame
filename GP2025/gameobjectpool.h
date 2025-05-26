#ifndef _GAMEOBJECTPOOL_H__
#define _GAMEOBJECTPOOL_H__

using namespace std;

#include <vector>

class GameObject;


class GameObjectPool {
public:
	GameObjectPool(const GameObject& prototype, size_t initialSize = 10);
	~GameObjectPool();

	GameObject* getObject();
	void release(GameObject* obj);
	void resize(size_t newSize);
	size_t usedCount() const;
	size_t totalCount() const;
	GameObject* getObjectAtIndex(size_t index);
	bool hasAvailableObjects() const;
	void add(GameObject* obj);

	void CleanUp();

protected:
private:
	GameObjectPool(const GameObjectPool& gameobjectpool);
	GameObjectPool& operator=(const GameObjectPool& gameobjectpool);

public:
protected:
private:
	struct Pool {
		GameObject* obj;
		bool inUse;
	};

	vector<Pool> pool;
	GameObject* m_prototype;
	size_t m_initialSize;
};

#endif // !_GAMEOBJECTPOOL_H__
