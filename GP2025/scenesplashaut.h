#ifndef SCENESPLASHAUT_H
#define SCENESPLASHAUT_H

#include "scene.h"

class Renderer;
class Sprite;

class SceneSplashAUT : public Scene {
public:
	SceneSplashAUT();
	virtual ~SceneSplashAUT();
	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

	bool getStatus();

protected:
private:
	SceneSplashAUT(const SceneSplashAUT& scenesplashAUT);
	SceneSplashAUT& operator=(const SceneSplashAUT& scenesplashAUT);

	//member data

public:

protected:
	Sprite* m_pAUT;
	float m_alpha;
	bool m_increase = true;
	bool m_done = false;

private:
};

#endif // SCENESPLASHAUT_H