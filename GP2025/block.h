#ifndef __BLOCK_H__
#define __BLOCK_H__

#include "vector2.h"
#include "gameobject.h"

class Renderer;
class Sprite;

class Block : public GameObject {
public:
	Block();
	virtual ~Block();

	bool Initialise(Renderer& renderer, int depth);
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

protected:
	void GetBlockType(int& depth, const char*& filepath);

private:
	Block(const Block& edge);
	Block& operator=(const Block& edge);

public:

protected:
	Vector2 m_position;
	Sprite* m_sprite;

	bool m_active;

	int m_depth;
	const char* m_filepath;

private:

};

#endif // !__BLOCK_H__
