#ifndef __SCENETITLESCREEN_H__
#define __SCENETITLESCREEN_H__

#include "scene.h"

class Sprite;
class Texture;
class Renderer;
class InputSystem;

class SceneTitlescreen : public Scene
{
	// Member methods
public:
	SceneTitlescreen();
	virtual ~SceneTitlescreen();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

private:
	SceneTitlescreen(const SceneTitlescreen& scene);
	SceneTitlescreen& operator=(const SceneTitlescreen& scene);

	bool IsMouseOverSprite(InputSystem& inputSystem, Sprite* sprite);

	// Member data
protected:
	Texture* m_pStartBtnTexture;
	Texture* m_pExitBtnTexture;

	Sprite* m_pStartBtnSprite;
	Sprite* m_pExitBtnSprite;

	Sprite* m_pBackgroundSprite;

	bool m_bisMouseOverStart;
	bool m_bisMouseOverExit;

	float m_screenWidth;
	float m_screenHeight;

	// White
	const float m_defaultRed = 1.0f;
	const float m_defaultGreen = 1.0f;
	const float m_defaultBlue = 1.0f;

	// Orange
	const float m_hoverRed = 1.0f;
	const float m_hoverGreen = 0.65f;
	const float m_hoverBlue = 0.0f;
};

#endif // __SCENETITLESCREEN_H__
