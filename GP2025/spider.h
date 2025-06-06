#ifndef __SPIDER_H__
#define __SPIDER_H__

#include "vector2.h"
#include "gameobject.h"

class Renderer;
class AnimatedSprite;

class Spider : public GameObject {
public:
	Spider();
	virtual ~Spider();

	bool Initialise(Renderer& renderer, const char* filepath);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	Vector2& Position();
	void SetColour(float red, float green, float blue);

	int GetSpriteHeight();
	virtual int GetSpriteWidth() const override;
	virtual GameObject* Create() const override;
	virtual bool IsActive() const override;
	virtual void Reset() override;

	void SetScale(float scale);
	void SetAlive(bool alive);

protected:

private:
	Spider(const Spider& spider);
	Spider& operator=(const Spider& spider);

public:

protected:
	Vector2 m_position;
	AnimatedSprite* m_sprite;

	bool m_alive;

private:

};

#endif // !__SPIDER_H__
