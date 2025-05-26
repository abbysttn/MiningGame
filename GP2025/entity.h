// COMP710 GP Framework 2022
#ifndef ENTITY_H
#define ENTITY_H
// Local includes:
#include "entity.h"
#include "vector2.h"
// Forward declarations:
class Renderer;
class Sprite;
class AnimatedSprite;
// Class declaration:
class Entity
{
	// Member methods:
public:
	Entity();
	virtual ~Entity();
	bool Initialise(Renderer& renderer);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	void Rotate(float direction);
	bool IsAlive() const;
	void SetDead();
	float GetRadius();
	Vector2 GetFacingDirection();
	Vector2& GetPosition();
	Vector2& GetVelocity();
	bool IsCollidingWith(Entity& toCheck);
protected:
private:
	Entity(const Entity& entity);
	Entity& operator=(const Entity& entity);
	// Member data:
public:
protected:
	Sprite* m_pSprite;
	AnimatedSprite* m_pAnimSprite;
	Vector2 m_position;
	Vector2 m_velocity;
	bool m_bAlive;
	bool m_bOwnsSprite = true;
private:
};
#endif // ENTITY_H
