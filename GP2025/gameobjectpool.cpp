#include "gameobjectpool.h"
#include "gameobject.h"

#include <stdexcept>


GameObjectPool::GameObjectPool(const GameObject& prototype, size_t initialSize) : m_prototype(prototype.Create()), m_initialSize(initialSize)
{
	try {
		pool.resize(initialSize);
		for (auto& entry : pool) {
			entry.obj = m_prototype->Create();
			entry.inUse = false;
			if (!entry.obj) throw std::bad_alloc();
		}
	}
	catch (...) {
		CleanUp();
		throw;
	}
}

GameObjectPool::~GameObjectPool() 
{
	CleanUp();
	delete m_prototype;
	m_prototype = nullptr;
}

GameObject* GameObjectPool::getObject()
{
	for (auto& obj : pool) {
		if (!obj.inUse) {
			obj.inUse = true;
			obj.obj->Reset();
			obj.obj->active = true;
			return obj.obj;
		}
	}

	size_t newSize = pool.size() * 2;
	resize(newSize);

	for (auto& obj : pool) {
		if (!obj.inUse) {
			obj.inUse = true;
			obj.obj->Reset();
			obj.obj->active = true;
			return obj.obj;
		}
	}

	throw runtime_error("Pool failed to expand.");
}

void GameObjectPool::release(GameObject* obj)
{
	for (auto& entry : pool) {
		if (entry.obj == obj) {
			entry.inUse = false;
			entry.obj->Reset();
			return;
		}
	}

	throw invalid_argument("Object not from this pool.");
}

void GameObjectPool::resize(size_t newSize)
{
	if (newSize < pool.size()) {
		size_t inUseCount = 0;

		for (const auto& entry : pool) {
			if (entry.inUse) {
				inUseCount++;
			}
		}

		if (newSize < inUseCount) {
			throw runtime_error("Cannot shrink pool below used objects.");
		}

		pool.resize(newSize);
	}
	else if (newSize > pool.size()) {
		size_t add = newSize - pool.size();

		for (size_t i = 0; i < add; i++) {
			Pool newEntry;
			newEntry.inUse = false;
			newEntry.obj = m_prototype->Create();
			newEntry.obj->Reset();
			pool.push_back(newEntry);
		}
	}
}

size_t GameObjectPool::usedCount() const
{
	size_t count = 0;

	for (const auto& entry : pool) {
		if (entry.inUse) {
			count++;
		}
	}

	return count;
}

size_t GameObjectPool::totalCount() const
{
	return pool.size();
}

GameObject* GameObjectPool::getObjectAtIndex(size_t index)
{
	if (index >= pool.size()) return nullptr;
	return pool[index].obj;
}

bool GameObjectPool::hasAvailableObjects() const
{
	for (const auto& entry : pool) {
		if (!entry.inUse) {
			return true;
		}
	}
	return false;
}

void GameObjectPool::add(GameObject* obj)
{
	for (auto& entry : pool) {
		if (entry.obj == obj) {
			entry.inUse = true;
			return;
		}
	}
}

void GameObjectPool::CleanUp()
{
	for (auto& entry : pool) {
		delete entry.obj;
		entry.obj = nullptr;
	}
	pool.clear();
}
