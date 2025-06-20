#ifndef __GRIDSTATE_H__
#define __GRIDSTATE_H__

#include "grid.h"
#include "block.h"
#include "vector"
#include "player.h"

class GameObjectPool;
class Renderer;
class Player;

using namespace std;

class GridState {
public:
	GridState() = default;
	~GridState() = default;

	static GridState& GetInstance() {
		static GridState instance;
		return instance;
	}

	void CreateGrid(Renderer& renderer, float backgroundScale, bool isRegular);
	GameObjectPool* GetPool();

	void BreakBlock(Vector2 position, char direction, Player* player);

	void ResetGrid();

	void ProcessGrid(float deltaTime, InputSystem& inputSystem);
	void DrawGrid(Renderer& renderer);

	bool CheckCollision(Box& box);
	bool CheckHazards();
	bool CheckFood();

	float GetTileSize();

	int GetDirt() const { return m_dirtCount; }

	int GetStone() const { return m_stoneCount; }

	int GetGem() const { return m_gemCount; }

	void SetPlayer(Player* player) { m_pPlayer = player; }
	void IsPlayerTouchingFood(const Vector2& playerPosition, float bgWidth, float bgHeight, float rendererHeight);

	bool CheckBlockDig();
	bool CheckBlockBreak();
	int GetLastBlockType();
	Vector2 GetBrokenBlockPos();
	bool IsBlockBroken();
	bool SpiderSpawn();

	Vector2 GetBlockSize();
	Vector2 GetBrokenBlockGridPos();

	void SetMiningStrength(int strength);

protected:

private:
	GridState(const GridState& gridState);
	GridState& operator=(const GridState& gridState);

public:

protected:
	Grid* m_gameGrid;

	bool m_blockBroken = false;
	Vector2 m_brokenBlockPos;
	Vector2 m_brokenBlockGridPos;
	int m_brokenBlockTile;
	Vector2 m_lastBrokenPos;

	bool m_touchingHazard;


	bool update1 = true;

	int m_dirtCount = 0;
	int m_stoneCount = 0;
	int m_gemCount = 0;

	Player* m_pPlayer = nullptr;

private:

	bool m_digBlock;
	bool m_breakBlock;
	int m_lastBlockType;
	bool m_touchingFood;
};

#endif // !__GRIDSTATE_H__
