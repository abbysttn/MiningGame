#ifndef __GRIDSTATE_H__
#define __GRIDSTATE_H__

#include "grid.h"
#include "block.h"
#include "vector"

class GameObjectPool;
class Renderer;

using namespace std;

class GridState {
public:
	GridState() = default;
	~GridState() = default;

	static GridState& GetInstance() {
		static GridState instance;
		return instance;
	}

	void CreateGrid(Renderer& renderer);
	GameObjectPool* GetPool();

	void BreakBlock(Block* block);
	void StopBreakingBlock(Block* block);

	void ResetGrid();

	char GetBlockType(Block* block);

	void ProcessGrid(float deltaTime, InputSystem& inputSystem);
	void DrawGrid(Renderer& renderer);

	bool CheckCollision(Box& box);

protected:

private:
	GridState(const GridState& gridState);
	GridState& operator=(const GridState& gridState);

public:

protected:
	//unordered_map<Block*, bool> m_itemStates; maybe for keeping track of broken blocks
	Grid* m_gameGrid;

	Block* m_collidingBlock;

private:
};

#endif // !__GRIDSTATE_H__
