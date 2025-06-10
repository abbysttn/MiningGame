#ifndef __GRID_H__
#define __GRID_H__

#include "scene.h"
#include <string>
#include "vector2.h"
#include <memory>

using namespace std;

class Scene;
class Renderer;
class Sprite;
class GameObjectPool;
class Block;

class QuadTree;
struct Box;

class Grid : public Scene {
public:
	Grid();
	virtual ~Grid();
	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

	float GetTileSize() { return m_tileSize; }

	QuadTree& GetCollisionTree();

	Vector2 GetBlockSize();
	Block* GetBlockFromGrid(const Vector2 position) const;

	int GetRows();
	int GetColumns();

	Vector2 GetScreenOffsets();

	void SetBackgroundHeight(float height);
	void SetGridType(bool regular);

	Vector2 GetPlayerStartPos();


protected:
	bool InitObjects(Renderer& renderer, size_t x, size_t y);
	bool InitCutsceneObjects(Renderer& renderer, size_t x, size_t y);
private:
	Grid(const Grid& grid);
	Grid& operator=(const Grid& grid);

public:

protected:

private:
	GameObjectPool* m_grid;

	unique_ptr<QuadTree> m_collisionTree;

	float m_tileSize;
	float m_backgroundHeight;
	bool m_isRegular;

	int m_cols = 20;
	int m_rows = 30;
	int m_count = m_rows * m_cols;

	float screenOffsetX;
	float screenOffsetY;

	Vector2 m_blockSize;

	Vector2 m_playerStartPos;

};

#endif // !__GRID_H__
