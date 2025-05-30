#ifndef _COLLISIONHELPER_H__
#define _COLLISIONHELPER_H__

#include "vector2.h"
#include "gameobject.h"
#include "quadtree.h"

class GameObject;
struct Box;

class CollisionHelper {
public:
	CollisionHelper() {}
	~CollisionHelper() {}

	static bool IsColliding(const Box& a, const Box& b) {
		return a.x < b.x + b.width &&
			a.x + a.width > b.x &&
			a.y < b.y + b.height &&
			a.y + a.height > b.y;
	}

	static Vector2 CalcPushDistance(const Box& a, const Box& b) {
		Vector2 aCenter(a.x + a.width / 2.0f, a.y + a.height / 2.0f);
		Vector2 bCenter(b.x + b.width / 2.0f, b.y + b.height / 2.0f);

		Vector2 direction(aCenter.x - bCenter.x, aCenter.y - bCenter.y);

		float length = GetLength(direction);

		if (length != 0) {
			direction.x /= length;
			direction.y /= length;
		}

		return direction;
	}

	static float GetLength(Vector2 direction) {
		float length = sqrtf(direction.x * direction.x + direction.y * direction.y);

		return length;
	}

};

#endif // !_COLLISIONHELPER_H__
