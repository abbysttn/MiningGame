#ifndef SCENESPLASHFMOD_H
#define SCENESPLASHFMOD_H

#include "scene.h"

class Renderer;
class Sprite;

class SceneSplashFMOD : public Scene {
public:
	SceneSplashFMOD();
	virtual ~SceneSplashFMOD();
	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

	bool getStatus();

protected:
private:
	SceneSplashFMOD(const SceneSplashFMOD& scenesplashfmod);
	SceneSplashFMOD& operator=(const SceneSplashFMOD& scenesplashfmod);

	//member data

public:

protected:
	Sprite* m_pFMOD;
	float m_alpha;
	bool m_increase = true;
	bool m_done = false;

private:
};

#endif // SCENESPLASHFMOD_H