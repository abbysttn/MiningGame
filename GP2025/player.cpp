#include "player.h"
#include "renderer.h"
#include "sprite.h"
#include "animatedsprite.h"

#include "gridstate.h"
#include "quadtree.h"

#include "vector2.h"
#include "xboxcontroller.h"

#include <algorithm>
#include <string>
#include "logmanager.h"


Player::Player()
    : m_speed(400.0f)
    , m_health(100.0f)
    , m_maxHealth(100.0f)
    , m_stamina(100.0f)
    , m_maxStamina(100.0f)
    , m_jumpHeightMultiplier(1.0f)
    , m_miningStrengthLevel(1)
    , m_headlampLevel(1)
    , m_oxygen(100.0f)
    , m_oxygenTimer(0.0f)
{
    m_inventory[ResourceType::DIRT] = 0;
    m_inventory[ResourceType::STONE] = 0;
    m_inventory[ResourceType::GEM] = 0;
}

Player::~Player()
{
    delete m_pIdleSprite;
    delete m_pJumpSprite;
    delete m_pMineSprite;
	m_pIdleSprite = nullptr;
	m_pJumpSprite = nullptr;
	m_pMineSprite = nullptr;
}

bool Player::Initialise(Renderer& renderer)
{
    m_pRenderer = &renderer;

	LoadAnimatedSprites();

    if (m_pAnimSprite == nullptr)
    {
        return false;
    }

    m_position = { static_cast<float>(renderer.GetWidth()/2), static_cast<float>(renderer.GetHeight())*0.3f};
    m_screenX = static_cast<float>(renderer.GetWidth() / 2);
    m_screenY = 20.0f;
    m_bAlive = true;
    m_invincibilityTimer = INVINSIBILITY_DURATION;

    //responsive jump height for different screen sizes
    m_jumpHeight = std::sqrtf(2.0f * GRAVITY * (GridState::GetInstance().GetBlockSize().y * 2.3f));
    return true;
}

void Player::Process(float deltaTime, InputSystem& inputSystem)
{
    if (m_invincibilityTimer > 0.0f)
    {
        m_invincibilityTimer -= deltaTime;
    }

    Vector2 direction(0.0f, 0.0f);

    bool staminaRepletion = GridState::GetInstance().CheckFood();

    if (staminaRepletion) 
    {
        SetCurrentStamina(m_stamina + deltaTime * 3.0f);
    }
    else 
    {
        SetCurrentStamina(m_stamina - (deltaTime / 2));
    }

    if (GetCurrentStamina() <= 0.0f)
    {
        LogManager::GetInstance().Log("DEAD!");
    }

    if (staminaRepletion) 
    {
        // So that it works when the player upgs their stamina otherwise it caps at 100.
        float replenishStamValue = GetCurrentStamina() + (20.0f * deltaTime);
        SetCurrentStamina(replenishStamValue);
    }

	if (m_stamina <= 0.0f) {
        m_canMine = false;
	}
    else {
		m_canMine = true;
    }

	if (!IsInvincible() && m_health <= 0.0f) {
		HandleDeath(1);
        //return;
	}

    // Oxygen depletion logic
    m_oxygenTimer += deltaTime;

	// Reduce oxygen by 1% every 3 seconds
    if (m_oxygenTimer >= 3.0f) {
        m_oxygen -= 1.0f; 
        if (m_oxygen < 0.0f) m_oxygen = 0.0f;
        m_oxygenTimer = 0.0f;
    }

	if (m_oxygen <= 0.0f) {
        HandleDeath(2);
        //return;
	}

    if (m_noClip) {
        if (IsKeyHeld(inputSystem, SDL_SCANCODE_W))    direction.y -= 1.0f;
        if (IsKeyHeld(inputSystem, SDL_SCANCODE_S))  direction.y += 1.0f;
    }
    if (IsKeyHeld(inputSystem, SDL_SCANCODE_A))  direction.x -= 1.0f;
    if (IsKeyHeld(inputSystem, SDL_SCANCODE_D)) direction.x += 1.0f;


    m_isMining = false;
    char breakDirection = 0;
    if (IsKeyHeld(inputSystem, SDL_SCANCODE_UP)) { breakDirection = 'U'; m_isMining = true; }
    if (IsKeyHeld(inputSystem, SDL_SCANCODE_DOWN)) { breakDirection = 'D'; m_isMining = true; }
    if (IsKeyHeld(inputSystem, SDL_SCANCODE_LEFT)) { breakDirection = 'L'; m_isMining = true; }
    if (IsKeyHeld(inputSystem, SDL_SCANCODE_RIGHT)) { breakDirection = 'R'; m_isMining = true; }

	if (m_isMining && !m_noClip && m_animationState != MINE) {
		m_pAnimSprite = m_pMineSprite;
		m_animationState = MINE;
		m_pMineSprite->Animate();
	}
	else if (!m_isMining && m_animationState == MINE) {
		m_pAnimSprite = m_pIdleSprite;
		m_animationState = IDLE;
        m_pAnimSprite->Animate();
	}

    //controller input
    if (inputSystem.GetNumberOfControllersAttached() > 0) {
        XboxController* controller = inputSystem.GetController(0);
        if (controller != nullptr && controller->IsConnected())
        {

			// Left Stick - Movement
            Vector2 leftStick = controller->GetLeftStick();

            const float MAX_AXIS = 32768.0f;
            const float DEADZONE = 0.2f;

            float lx = leftStick.x / MAX_AXIS;
            if (std::abs(lx) > DEADZONE) {
                direction.x = lx;
            }

            // Right Stick - Mining
			Vector2 rightStick = controller->GetRightStick();
            
            float rx = rightStick.x / MAX_AXIS;
			float ry = rightStick.y / MAX_AXIS;

			if (std::abs(rx) > DEADZONE || std::abs(ry) > DEADZONE) {
				m_isMining = true;

				if (std::abs(rx) > std::abs(ry)) {
					breakDirection = (rx > 0.0f) ? 'R' : 'L';
				}
				else {
					breakDirection = (ry > 0.0f) ? 'D' : 'U';
				}
			}

            // A Button - Jump
            if (!m_noClip && m_OnGround && controller->GetButtonState(SDL_CONTROLLER_BUTTON_A) == BS_HELD) {
				m_Velocity.y = -(m_jumpHeight * m_jumpHeightMultiplier);
				m_OnGround = false;

				// Change animation to jump/fall
				if (m_animationState != JUMP) {
					m_pAnimSprite = m_pJumpSprite;
					m_animationState = JUMP;
					m_pJumpSprite->SetCurrentFrame(0);
					m_pJumpSprite->Animate();
				}
            }
        }
    }

    // Perform mining input
    if (m_canMine && !m_noClip && breakDirection != 0)
    {
        GridState::GetInstance().BreakBlock(m_position, breakDirection, this);
    }

    //normalising to prevent faster diagonal movement
    if (direction.x != 0.0f || direction.y != 0.0f)
    {
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        direction.x /= length;
        direction.y /= length;
    }

    if (m_noClip){
        m_velocity.y = 0.0f;
        m_OnGround = false;
    }

    const int screenWidth = m_pRenderer->GetWidth();
    const int screenHeight = m_pRenderer->GetWorldHeight();

    const int spriteHalfWidth = m_pAnimSprite->GetWidth() / 2;
    const int spriteHalfHeight = m_pAnimSprite->GetHeight() / 2;

	float spriteW = static_cast<float>(m_pAnimSprite->GetWidth());
	float spriteH = static_cast<float>(m_pAnimSprite->GetHeight());

    // Gravity
	if (!m_noClip) {
        // If on ground and space is pressed, jump
        if (m_OnGround && IsKeyHeld(inputSystem, SDL_SCANCODE_SPACE)) {
			m_Velocity.y = -(m_jumpHeight * m_jumpHeightMultiplier);
			m_OnGround = false;

            // Change animation to jump/fall
            if (m_animationState != JUMP) {
                m_pAnimSprite = m_pJumpSprite;
                m_animationState = JUMP;
                m_pJumpSprite->SetCurrentFrame(0);
                m_pJumpSprite->Animate();
            }
        }

        // If not on ground, apply gravity
		if (!m_OnGround) {
			m_Velocity.y += GRAVITY * deltaTime;

		}
        else {
			// If on ground, reset vertical velocity
			m_Velocity.y = 0.0f;

			// Change animation to idle 
            if (m_isMining == false && m_animationState != IDLE) {
                m_pAnimSprite = m_pIdleSprite;
                m_animationState = IDLE;
            }

        }
	}

    //move the player
    Vector2 testPos = m_position;

    testPos += direction * m_speed * deltaTime;
	testPos.y += m_Velocity.y * deltaTime;

    float paddingX = (m_pAnimSprite->GetWidth() / 3.3f);
    float paddingY = (m_pAnimSprite->GetHeight() / 4.7f);
    Box testBoxX(testPos.x + paddingX, m_position.y + paddingY, (float)m_pAnimSprite->GetWidth() * 0.45f, (float)m_pAnimSprite->GetHeight() * 0.7f);

    if (m_noClip || !GridState::GetInstance().CheckCollision(testBoxX)) {
        m_position.x = testPos.x;
    }

    Box testBoxY(m_position.x + paddingX, testPos.y + paddingY, (float)m_pAnimSprite->GetWidth() * 0.45f, (float)m_pAnimSprite->GetHeight() * 0.7f);

    if (m_noClip || !GridState::GetInstance().CheckCollision(testBoxY)) {
        m_position.y = testPos.y;
        m_OnGround = false;
    }
    else {
        if (m_Velocity.y > 0.0f) {
            // Moving down - landed on ground
            m_OnGround = true;
        }

        m_Velocity.y = 0.0f;
    }

    //check if player is colliding with a hazard
    if (!IsInvincible() && !m_noClip && GridState::GetInstance().CheckHazards()) {
        m_health = 0.0f;
        HandleDeath(3);
    }

    //clamp to screen with halfWidth, to prevent clipping outside screen
    float wallMarginX = screenWidth * 0.005f;  //0.5% horizontal margin (for the walls)
    float wallMarginY = screenHeight * 0.00f; //PLayer can't move past the top 5%

    float minX = wallMarginX + spriteHalfWidth;
    float maxX = screenWidth - wallMarginX - spriteHalfWidth;
    float minY = wallMarginY + spriteHalfHeight;
    maxHeight = static_cast<float>(screenHeight - spriteHalfHeight);

    m_position.x = std::max(minX, std::min(maxX, m_position.x));
    m_position.y = std::max(minY, std::min(maxHeight, m_position.y));

	if (direction.x < 0.0f) m_facingLeft = true;
	else if (direction.x > 0.0f) m_facingLeft = false;

    m_pAnimSprite->Process(deltaTime);
}

void Player::Draw(Renderer& renderer)
{
    if (m_pAnimSprite && m_bAlive)
    {
        m_pAnimSprite->SetX(static_cast<int>(m_position.x));
        m_pAnimSprite->SetY(static_cast<int>(m_position.y));
        m_pAnimSprite->Draw(renderer, m_facingLeft);
    }
}

float Player::GetPlayerHeight(){
    return static_cast<float>(m_pAnimSprite->GetHeight());
}
float Player::GetPlayerWidth() {
    return static_cast<float>(m_pAnimSprite->GetWidth());
}


bool Player::IsKeyHeld(InputSystem& input, SDL_Scancode key)
{
    return input.GetKeyState(key) == BS_HELD;
}

void Player::LoadAnimatedSprites() {
	m_pIdleSprite = m_pRenderer->CreateAnimatedSprite("../assets/Idle.png");
	m_pJumpSprite = m_pRenderer->CreateAnimatedSprite("../assets/Jump.png");
	m_pMineSprite = m_pRenderer->CreateAnimatedSprite("../assets/Swing.png");

    float scale = GridState::GetInstance().GetTileSize() / 35.0f;

    m_pIdleSprite->SetupFrames(32, 32);
	m_pIdleSprite->SetLooping(true);
	m_pIdleSprite->SetFrameDuration(0.1f);
	m_pIdleSprite->Animate();
    m_pIdleSprite->SetScale(scale);

	m_pJumpSprite->SetupFrames(32, 32);
	m_pJumpSprite->SetLooping(false);
	m_pJumpSprite->SetFrameDuration(0.05f);
	m_pJumpSprite->Animate();
	m_pJumpSprite->SetScale(scale);

	m_pMineSprite->SetupFrames(32, 32);
	m_pMineSprite->SetLooping(true);
	m_pMineSprite->SetFrameDuration(0.1f);
	m_pMineSprite->Animate();
	m_pMineSprite->SetScale(scale);

    // Default
    m_pAnimSprite = m_pIdleSprite;
	m_animationState = IDLE;
}

bool Player::IsAtBottom()
{
    return m_position.y >= maxHeight;
}

void Player::SetMaxHealth(float newMaxHealth)
{
    m_maxHealth = newMaxHealth;

    if (m_health > m_maxHealth)
    {
        m_health = m_maxHealth;
    }
}

void Player::SetMaxStamina(float newMaxStamina) 
{
    m_maxStamina = newMaxStamina;

    if (m_stamina > m_maxStamina) 
    {
        m_stamina = m_maxStamina;
    }
}

void Player::AddResource(ResourceType type, int amount) 
{
    m_inventory[type] += amount;
    if (m_inventory[type] < 0) 
    {
        m_inventory[type] = 0;
    }
}

bool Player::HasResource(ResourceType type, int amount) const 
{
    auto it = m_inventory.find(type);
    if (it != m_inventory.end()) 
    {
        return it->second >= amount;
    }
    return false; 
}

bool Player::RemoveResource(ResourceType type, int amount) 
{
    if (HasResource(type, amount)) 
    {
        m_inventory[type] -= amount;
        return true;
    }
    return false;
}

int Player::GetResourceCount(ResourceType type) const 
{
    auto it = m_inventory.find(type);
    if (it != m_inventory.end()) 
    {
        return it->second;
    }
    return 0;
}

void Player::HandleDeath(int deathType) 
{
    SetDead();
	switch (deathType) 
    {
	case 1: // Health depleted
		LogManager::GetInstance().Log("Player died due to health depletion.");
		break;
	case 2: // Oxygen depleted
		LogManager::GetInstance().Log("Friend died due to oxygen depletion.");
		break;
    case 3: // Player hit a hazard
        LogManager::GetInstance().Log("Player died due to a underground hazard.");
        break;
	default:
		LogManager::GetInstance().Log("Player died for an unknown reason.");
		break;
	}
}

void Player::SetMiningStrengthLevel(int level)
{
    m_miningStrengthLevel = level;
    GridState::GetInstance().SetMiningStrength(m_miningStrengthLevel);
}

void Player::Reset()
{
    m_bAlive = true;
    m_facingLeft = false;
    m_isMining = false;
    m_canMine = true;
    m_instantMine = false;
    m_infiniteResources = false;

    m_speed = 400.0f;
    m_health = 100.0f;
	m_maxHealth = 100.0f;
    m_stamina = 100.0f;
    m_maxStamina = 100.0f;
    m_jumpHeightMultiplier = 1.0f;
    m_miningStrengthLevel = 1;
	m_headlampLevel = 1;
    m_oxygen = 100.0f;
    m_oxygenTimer = 0.0f;
	m_invincibilityTimer = INVINSIBILITY_DURATION;

    m_inventory[ResourceType::DIRT] = 0;
    m_inventory[ResourceType::STONE] = 0;
    m_inventory[ResourceType::GEM] = 0;

    m_position = { m_screenX, m_screenY};

    m_Velocity = { 0.0f, 0.0f };
    m_OnGround = false;
    m_pAnimSprite = m_pIdleSprite;
    m_animationState = IDLE;
}

bool Player::IsInvincible() const
{
	return m_invincibilityTimer > 0.0f;
}