#ifndef __BLOCK_H__
#define __BLOCK_H__

#include "vector2.h"
#include "gameobject.h"

class Renderer;
class AnimatedSprite;

class Block : public GameObject {
public:
	Block();
	virtual ~Block();

	bool Initialise(Renderer& renderer, int depth, int x);
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
	void SetActive(bool active);

	void SetBreakable(bool canBreak);
	bool CanBreak();

	void BreakBlock();
	bool BlockBroken();
	char GetBlockType();
	int GetResourceAmount();
	bool ResourcesGiven();

protected:
	void GetBlockType(int& depth, const char*& filepath, int x);

private:
	Block(const Block& edge);
	Block& operator=(const Block& edge);

public:

protected:
	Vector2 m_position;
	AnimatedSprite* m_sprite;

	bool m_active;

	int m_depth;
	const char* m_filepath;
	char m_blockType;
	bool m_resourceGiven = false;
	bool m_canBreak = true;

	bool m_isBroken;
	bool m_isBreaking = false;
	int m_currentBlockStatus = 0;
	float m_animatingTime = 0.5f;
	float m_currentTime = 0.0f;

private:

};

#endif // !__BLOCK_H__
