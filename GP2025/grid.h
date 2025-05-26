#ifndef __GRID_H__
#define __GRID_H__

#include "scene.h"
#include <string>
#include "vector2.h"

using namespace std;

class Scene;
class Renderer;
class Sprite;
class GameObjectPool;

class EnemySpawner;

class Grid : public Scene {
public:
	Grid();
	virtual ~Grid();
	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();


protected:
	bool InitObjects(Renderer& renderer, size_t x, size_t y);
private:
	Grid(const Grid& grid);
	Grid& operator=(const Grid& grid);

public:

protected:

private:
	GameObjectPool* m_grid;

	float m_tileSize;

	int m_cols = 10;
	int m_rows = 5;
	int m_count = 50;

	float screenOffsetX;
	float screenOffsetY;

};

#endif // !__GRID_H__
