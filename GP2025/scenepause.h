#ifndef __SCENEPAUSE_H__
#define __SCENEPAUSE_H__

// Local includes
#include "scene.h"
#include "soundsystem.h"

// FOward declarations
class Sprite;
class Texture;
class Renderer;
class InputSystem;
class Font;

class ScenePause : public Scene
{
	// Member methods
public:
	ScenePause();
	virtual ~ScenePause();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

	void OnEnter() override;
	void OnExit() override;

private:
	ScenePause(const ScenePause& scene);
	ScenePause& operator=(const ScenePause& scene);

	bool IsMouseOverSprite(InputSystem& inputSystem, Sprite* sprite);
	void SetButtonTints(Sprite* button, bool isMouseOver);

	// Member data
public:

protected:
	Texture* m_pResumeBtnTexture;
	Texture* m_pQuitToTitleBtnTexture;
	Texture* m_pExitGameBtnTexture;


	Sprite* m_pResumeBtnSprite;
	Sprite* m_pQuitToTitleBtnSprite;
	Sprite* m_pExitGameBtnSprite;

	Font* m_pButtonFont;

	float m_screenWidth;
	float m_screenHeight;

	// Button colors (white)
	const float m_defaultRed = 1.0f;
	const float m_defaultGreen = 1.0f;
	const float m_defaultBlue = 1.0f;

	// orange
	const float m_hoverRed = 1.0f;
	const float m_hoverGreen = 0.65f;
	const float m_hoverBlue = 0.0f;

	const float m_buttonYOffset = 60.0f;
	const float m_firstButtonYPosFactor = 0.4f;

	SoundSystem m_soundSystem;
};

#endif // __SCENEPAUSE_H__
