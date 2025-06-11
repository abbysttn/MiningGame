// --- START OF FILE scenepause.h ---

#ifndef __SCENEPAUSESCREEN_H__
#define __SCENEPAUSESCREEN_H__

#include "scene.h"
#include "soundsystem.h"

class Sprite;
class Texture;
class Renderer;
class InputSystem;
class Font;
class Vector2;

class ScenePauseScreen : public Scene
{
public:
	ScenePauseScreen();
	virtual ~ScenePauseScreen();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

	void OnEnter(bool reset);
	void OnExit();

private:
	ScenePauseScreen(const ScenePauseScreen& scene);
	ScenePauseScreen& operator=(const ScenePauseScreen& scene);

	bool IsMouseOverSprite(InputSystem& inputSystem, Sprite* sprite);

protected:
	// Buttonz
	Texture* m_pResumeBtnTexture;
	Sprite* m_pResumeBtnSprite;
	Texture* m_pToTitleBtnTexture;
	Sprite* m_pToTitleBtnSprite;
	Texture* m_pExitBtnTexture;
	Sprite* m_pExitBtnSprite;

	// Visuals
	Sprite* m_pBackgroundSprite;
	Font* m_pButtonFont;

	// State
	bool m_bisMouseOverResume;
	bool m_bisMouseOverToTitle;
	bool m_bisMouseOverExit;

	float m_screenWidth;
	float m_screenHeight;

	SoundSystem m_pSoundSystem;

	// Color constants for buttons (White)
	const float m_defaultRed = 1.0f;
	const float m_defaultGreen = 1.0f;
	const float m_defaultBlue = 1.0f;

	// Color constants for buttons (Orange)
	const float m_hoverRed = 1.0f;
	const float m_hoverGreen = 0.65f;
	const float m_hoverBlue = 0.0f;
};

#endif // __SCENEPAUSESCREEN_H__