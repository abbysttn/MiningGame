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

	void BreakBlock(Vector2 position, char direction);

	void ResetGrid();

	char GetBlockType(Block* block);

	void ProcessGrid(float deltaTime, InputSystem& inputSystem);
	void DrawGrid(Renderer& renderer);

	bool CheckCollision(Box& box);

	float GetTileSize();

	int GetDirt() const { return m_dirtCount; }

	int GetStone() const { return m_stoneCount; }

	int GetGem() const { return m_gemCount; }

protected:

private:
	GridState(const GridState& gridState);
	GridState& operator=(const GridState& gridState);

public:

protected:
	Grid* m_gameGrid;

	int m_dirtCount = 0;
	int m_stoneCount = 0;
	int m_gemCount = 0;

private:
};

#endif // !__GRIDSTATE_H__
