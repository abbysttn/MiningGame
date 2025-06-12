#ifndef _FALLINGROCKS_H__
#define _FALLINGROCKS_H__

#include "scene.h"
#include "gameobjectpool.h"
#include "vector2.h"
#include "vector"

using namespace std;

class Renderer;
class InputSystem;

class FallingRocks : public Scene {
public:
	FallingRocks();
	virtual ~FallingRocks();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

	void SetScale(float scale);
	void SetStartPos(Vector2 pos);
	void SetEndPos(Vector2 pos);

	void SetFalling(bool falling);

	bool GetStatus();

	void Reset();

protected:

private:
	FallingRocks(const FallingRocks& fallingRocks);
	FallingRocks& operator=(const FallingRocks& fallingRocks);

public:

protected:
	GameObjectPool* m_blocks;

	Vector2 m_startPos;
	Vector2 m_endPos;

	float m_scale = 40.0f;

	float m_fallSpeed = 0.0f;
	float m_gravity = 500.0f;
	bool m_falling;

	vector<float> m_fallDelays;

private:
};

#endif // !_FALLINGROCKS_H__
