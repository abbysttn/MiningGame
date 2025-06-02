#ifndef __SCENETITLESCREEN_H__
#define __SCENETITLESCREEN_H__

#include "scene.h"
#include "fmod.hpp"

class Sprite;
class Texture;
class Renderer;
class InputSystem;
class Font;

class SceneTitlescreen : public Scene
{
	// Member methods
public:
	SceneTitlescreen(FMOD::System* fmodSystem);
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
	Sprite* m_pTitleSprite;

	Font* m_pButtonFont;

	bool m_bisMouseOverStart;
	bool m_bisMouseOverExit;

	float m_screenWidth;
	float m_screenHeight;

	// White color
	const float m_defaultRed = 1.0f;
	const float m_defaultGreen = 1.0f;
	const float m_defaultBlue = 1.0f;

	// Orange color
	const float m_hoverRed = 1.0f;
	const float m_hoverGreen = 0.65f;
	const float m_hoverBlue = 0.0f;

	FMOD::System* m_pFMODSystem;
	FMOD::Sound* m_pClickSound;
	FMOD::Sound* m_pBgmSound;
	FMOD::Channel* m_pBgmChannel;

	// Layout for titlescreen
	const float m_titleTopMargin = 0.25f;
	const float m_titleButtonPadding = 50.0f;
	const float m_buttonPadding = 30.0f;
};

#endif // __SCENETITLESCREEN_H__
