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

	QuadTree& GetCollisionTree();


protected:
	bool InitObjects(Renderer& renderer, size_t x, size_t y);
private:
	Grid(const Grid& grid);
	Grid& operator=(const Grid& grid);

public:

protected:

private:
	GameObjectPool* m_grid;

	unique_ptr<QuadTree> m_collisionTree;

	float m_tileSize;

	int m_cols = 12;
	int m_rows = 100;
	int m_count = m_rows * m_cols;

	float screenOffsetX;
	float screenOffsetY;

};

#endif // !__GRID_H__
