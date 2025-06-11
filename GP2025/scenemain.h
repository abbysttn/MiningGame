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

	void DebugFunctions(InputSystem& inputSystem);

	int GetBackgroundHeight();

	void OnEnter(bool reset) override;
	void OnExit() override;

	void CheckCollision(Player* player, SpiderState* spider);

	void SetVisionLevel(int level);

	void LightEvent(float time);

	void ToggleGodMode() { m_godMode = !m_godMode; }

	bool IsFinished();
	bool GameWon();

	void Reset();
	void CleanUp();
	void Reinitialise();

private:

	float m_tileSize;
	int m_aboveGroundOffset = 8; // Change later to correct above ground size

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
	float m_lightEventTimer;
	float m_lightEventInterval;
	float m_lightEventSkipped;
	float m_flicktimer;

	int m_activeDrops;

	bool m_paused = false;
	bool m_gameOver = false;

	bool m_godMode = false;

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

	bool m_lightOn;

	bool m_munching;
	float m_munchTimer;

	//Particle System
	Sprite* m_pCoinSprite = nullptr;
	Sprite* m_pDirtSprite = nullptr;
	Sprite* m_pBreakBlockSprite = nullptr;
	Sprite* m_pWaterDropSprite = nullptr;
	Sprite* m_pVignetteSprite = nullptr;
	Sprite* m_pDarkVignetteSprite = nullptr;


	Sprite* m_pDirtPickupSprite = nullptr;
	Sprite* m_pStonePickupSprite = nullptr;
	Sprite* m_pGemPickupSprite = nullptr;


	std::list<ParticleSystem> m_particleSystems;
	std::unique_ptr<QuadTree> m_collisionTree;

};

#endif // __SceneMain_H__