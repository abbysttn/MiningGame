#ifndef __SCENEMAIN_H__
#define __SCENEMAIN_H__

#include "scene.h"
#include "player.h"
#include "sprite.h"
#include "animatedsprite.h"
#include "fmod.hpp"
#include "ui.h"

#include <vector>
#include <map>
#include <random>
#include <memory>
#include <list>
#include <string>

class Grid;

class SceneMain : public Scene
{
public:
	SceneMain(FMOD::System* pFMODSystem);
	~SceneMain();

	bool Initialise(Renderer& renderer) override;
	void Process(float deltaTime, InputSystem& inputSystem) override;
	void Draw(Renderer& renderer) override;
	void MoveCamera(Renderer& renderer);
	void DebugDraw() override;

	int GetBackgroundHeight();


private:
	Grid* m_grid;
	float m_tileSize;
	int m_aboveGroundOffset = 11; // Change later to correct above ground size

	float m_screenWidth;
	float m_screenHeight;
	Player* m_pPlayer;

	Renderer* m_pRenderer;
	Sprite* m_pMineBackground;
	FMOD::System* m_pFMODSystem;
	int m_screenX;
	float m_playerY;

	std::unique_ptr<UI> ui;
};

#endif // __SceneMain_H__