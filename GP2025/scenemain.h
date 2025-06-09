#ifndef __SCENEMAIN_H__
#define __SCENEMAIN_H__

#include "scene.h"
#include "player.h"
#include "sprite.h"
#include "animatedsprite.h"
#include "fmod.hpp"
#include "ui.h"

#include "soundsystem.h"

#include "upgrademanager.h"
#include "upgradestation.h"

#include "particle.h"
#include "particlesystem.h"

#include <vector>
#include <map>
#include <random>
#include <memory>
#include <list>
#include <string>

#include "spiderstate.h"
#include "gameobjectpool.h"

#include "quadtree.h"

class SceneMain : public Scene
{
public:
	SceneMain();
	~SceneMain();

	bool Initialise(Renderer& renderer) override;
	void Process(float deltaTime, InputSystem& inputSystem) override;
	void Draw(Renderer& renderer) override;
	void MoveCamera(Renderer& renderer);
	void DebugDraw() override;

	void SpawnWaterDrops();
	void ProcessParticles(float time);

	void TestingFeatures(InputSystem& inputSystem);

	int GetBackgroundHeight();

	void OnEnter() override;
	void OnExit() override;

	void CheckCollision(Player* player, SpiderState* spider);

	void SetVisionLevel(int level);


private:
	float m_tileSize;
	int m_aboveGroundOffset = 11; // Change later to correct above ground size

	float m_pVisionLevel;
	std::vector<float> m_visionLevels;

	float m_dirtParticleCooldown;

	float m_screenWidth;
	float m_screenHeight;
	Player* m_pPlayer;

	Renderer* m_pRenderer;
	Sprite* m_pMineBackground;
	int m_screenX;
	float m_playerY;
	float m_waterDropInterval = 1.0f;
	float m_timer;
	int m_activeDrops;

	bool m_paused = false;

	std::unique_ptr<UI> ui;

	SoundSystem m_soundSystem;

	GameObjectPool* m_testSpider;

	// Upgrade system
	UpgradeManager m_upgradeManager;
	std::vector<UpgradeStation> m_upgradeStations;
	UpgradeStation* m_pActiveUpgradeStation;

	bool m_showUpgradePrompt;
	std::string m_interationKeyLabel = "[E]"; 

	bool m_isUpgradeMenuUIVisible;

	//Particle System
	Sprite* m_pCoinSprite = nullptr;
	Sprite* m_pDirtSprite = nullptr;
	Sprite* m_pBreakBlockSprite = nullptr;
	Sprite* m_pWaterDropSprite = nullptr;
	Sprite* m_pVignetteSprite = nullptr;


	Sprite* m_pDirtPickupSprite = nullptr;
	Sprite* m_pStonePickupSprite = nullptr;
	Sprite* m_pGemPickupSprite = nullptr;


	std::list<ParticleSystem> m_particleSystems;
	std::unique_ptr<QuadTree> m_collisionTree;

};

#endif // __SceneMain_H__