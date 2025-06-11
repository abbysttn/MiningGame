#ifndef __SCENECONTROLSMENU_H__
#define __SCENECONTROLSMENU_H__

// Local includes
#include "scene.h"
#include "soundsystem.h"

class Sprite;
class Texture;
class Renderer;
class InputSystem;
class Font;

class SceneControlsMenu : public Scene
{
public:
	SceneControlsMenu();
	virtual ~SceneControlsMenu();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

private:
	SceneControlsMenu(const SceneControlsMenu& scene);
	SceneControlsMenu& operator=(const SceneControlsMenu& scene);

	bool IsMouseOverSprite(InputSystem& inputSystem, Sprite* sprite);

protected:
	// Buttonsss
	Texture* m_pKeyboardBtnTexture;
	Sprite* m_pKeyboardBtnSprite;
	Texture* m_pControllerBtnTexture;
	Sprite* m_pControllerBtnSprite;
	Texture* m_pBackBtnTexture;
	Sprite* m_pBackBtnSprite;

	// Visuals
	Sprite* m_pBackgroundSprite;
	Font* m_pButtonFont;

	// State
	bool m_bisMouseOverKeyboard;
	bool m_bisMouseOverController;
	bool m_bisMouseOverBack;

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

#endif // __SCENECONTROLSMENU_H__