#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "inputsystem.h"
//#include "renderer.h"
//#include "scenemain.h"
#include <cmath>
#include <map>
#include <algorithm>
#include "resourcetype.h"

enum PlayerAnimationState {
	IDLE,
	JUMP,
	MINE
};

class Player : public Entity {

public:
	Player();
	~Player();

	bool Initialise(Renderer& renderer);
	void Process(float deltaTime, InputSystem& inputSystem);
	void Draw(Renderer& renderer);
	bool IsKeyHeld(InputSystem& input, SDL_Scancode key);

	Vector2 GetPosition() const { return m_position; };

	// Health for upgrade
	float GetCurrentHealth() const { return m_health; }
	void SetCurrentHealth(float newHealth) { m_health = std::max(0.0f, std::min(newHealth, m_maxHealth)); }
	float GetMaxHealth() const { return m_maxHealth; }
	void SetMaxHealth(float newMaxHealth);

	// Headlamp upgrade
	int GetHeadlampLevel() const { return m_headlampLevel; }
	void SetHeadlampLevel(int level) { m_headlampLevel = level; }

	// Stamina for upgrades
	float GetCurrentStamina() const { return m_stamina; }
	void SetCurrentStamina(float newStamina) { m_stamina = std::max(0.0f, std::min(newStamina, m_maxStamina));	}
	float GetMaxStamina() const { return m_maxStamina; }
	void SetMaxStamina(float newMaxStamina);

	int GetDepth() const { return m_depth; }
	void SetDepth(int depth) { m_depth = depth; }

	float GetPlayerHeight();
	float GetPlayerWidth();

	// Resources
	void AddResource(ResourceType type, int amount);
	bool HasResource(ResourceType type, int amount) const;
	bool RemoveResource(ResourceType type, int amount);
	int GetResourceCount(ResourceType type) const;

	int GetDirt() const { return GetResourceCount(ResourceType::DIRT); }
	void SetDirt(int dirt) { m_inventory[ResourceType::DIRT] = std::max(0, dirt); }

	int GetStone() const { return GetResourceCount(ResourceType::STONE); }
	void SetStone(int stone) { m_inventory[ResourceType::STONE] = std::max(0, stone); }

	int GetGem() const { return GetResourceCount(ResourceType::GEM); }
	void SetGem(int gem) { m_inventory[ResourceType::GEM] = std::max(0, gem); }

	bool IsInvincible() const;

	// Oxygen (can be turned into a upgradeable stat)
	float GetOxygen() const { return m_oxygen; }
	void AddOxygen(float amount) { m_oxygen = std::max(0.0f, std::min(m_oxygen + amount, 100.0f)); }

	void SetNoClip(bool noClip) { m_noClip = noClip; }
	bool IsNoClip() const { return m_noClip; }

	void LoadAnimatedSprites();

	bool IsAtBottom();

	bool IsPlayerMining() { return m_isMining; }
	PlayerAnimationState GetCurrentState() { return m_animationState; }
	
	void HandleDeath(int deathType);

	int GetStaminaCost() const { return m_staminaCost; }

	// Jump Height
	float GetJumpHeightMultiplier() const { return m_jumpHeightMultiplier; }
	void SetJumpHeightMultiplier(float multiplier) { m_jumpHeightMultiplier = multiplier; }

	// Mining strength
	int GetMiningStrengthLevel() const { return m_miningStrengthLevel; }
	void SetMiningStrengthLevel(int level);

	void SetInfiniteResources(bool infinite) { m_infiniteResources = infinite; }
	bool IsInfiniteResources() const { return m_infiniteResources; }

	void SetInstantMine(bool instant) { m_instantMine = instant; }
	bool IsInstantMine() const { return m_instantMine; }

	void Reset();

private:
	float m_speed;
	Renderer* m_pRenderer;
	float m_heightMultiple = 3.0f; //The height multiple vs the height of the renderer. e.g. the renderer height is 1080, 3240 = 3*1080. NEEED to make this dynamic

	const float INVINSIBILITY_DURATION = 5.0f;
	float m_invincibilityTimer;

	// Player stats
	float m_health;
	float m_maxHealth;
	float m_stamina;
	float m_maxStamina;

	std::map<ResourceType, int> m_inventory;

	// Jumping and Gravity
	const float GRAVITY = 1000.0f; // Pixels per second squared

	float maxHeight;

	float m_oxygen;
	float m_oxygenTimer;
	int m_depth;

	// UPGRADABLE VALUES
	int m_staminaCost = 8;			 // Cost to mine a block
	float m_jumpHeight;	 // Jump Height
	float m_jumpHeightMultiplier; // Jump Height Multiplier for upgrades
	int m_miningStrengthLevel; // Mining Strength Level 
	int m_headlampLevel;
	// int m_miningSpeed = 1;

	Vector2 m_Velocity;
	bool m_OnGround = false;
	bool m_noClip = false;

	bool m_infiniteResources = false; 

	PlayerAnimationState m_animationState = IDLE;

	AnimatedSprite* m_pIdleSprite = nullptr;
	AnimatedSprite* m_pJumpSprite = nullptr;
	AnimatedSprite* m_pMineSprite = nullptr;

	bool m_playerEating;

	bool m_facingLeft = false;
	bool m_isMining = false;
	bool m_canMine = true;
	bool m_instantMine = false;

	float m_screenX;
	float m_screenY;
};

#endif // PLAYER_H