#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "inputsystem.h"
//#include "renderer.h"
//#include "scenemain.h"
#include <cmath>
#include <algorithm>

class Player : public Entity {

enum PlayerAnimationState {
	IDLE,
	JUMP,
	MINE
};

public:
	Player();
	~Player();

	bool Initialise(Renderer& renderer);
	void Process(float deltaTime, InputSystem& inputSystem);
	void Draw(Renderer& renderer);
	void HandleMovement(float deltaTime, InputSystem& inputSystem);
	void ClampToScreen();
	bool IsKeyHeld(InputSystem& input, SDL_Scancode key);

	Vector2 GetPosition() const { return m_position; };

	float GetHealth() const { return m_health; }
	void SetHealth(float newHealth){ m_health = std::max(0.0f, std::min(newHealth, 100.0f)); }

	float GetStamina() const { return m_stamina; }
	void SetStamina(float newStamina) { m_stamina = std::max(0.0f, std::min(newStamina, 100.0f));	}
	
	int GetDepth() const { return m_depth; }
	void SetDepth(int depth) { m_depth = depth; }

	float GetPlayerHeight();
	int GetDirt() const { return m_dirtCount; }
	void SetDirt(int dirt) { m_dirtCount = std::max(0, dirt); }

	int GetStone() const { return m_stoneCount; }
	void SetStone(int stone) { m_stoneCount = std::max(0, stone); }

	int GetGem() const { return m_gemCount; }
	void SetGem(int gem) { m_gemCount = std::max(0, gem); }

	void SetNoClip(bool noClip);

	void LoadAnimatedSprites();

private:
	float m_speed;
	Renderer* m_pRenderer;
	float m_heightMultiple = 3.0f; //The height multiple vs the height of the renderer. e.g. the renderer height is 1080, 3240 = 3*1080. NEEED to make this dynamic

	float m_health;
	float m_stamina;
	int m_depth;

	int m_dirtCount = 0;
	int m_stoneCount = 0;
	int m_gemCount = 0;

	const float GRAVITY = 1000.0f; // Gravity in pixels per second squared
	const float JUMP_FORCE = 500.0f; // Force applied when jumping
	const float GROUND_Y = 700.0f; // Y position of temp ground

	Vector2 m_Velocity;
	bool m_OnGround = false;

	bool m_noClip = false; 

	PlayerAnimationState m_animationState = IDLE;

	AnimatedSprite* m_pIdleSprite = nullptr;
	AnimatedSprite* m_pJumpSprite = nullptr;
	AnimatedSprite* m_pMineSprite = nullptr;

	bool m_facingLeft = false;
};

#endif // PLAYER_H