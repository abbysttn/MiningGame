#include "player.h"
#include "renderer.h"
#include "sprite.h"
#include "animatedsprite.h"

#include "vector2.h"
#include "xboxcontroller.h"

#include <algorithm>


Player::Player()
    : m_speed(400.0f)
    , m_health(100.0f)
	, m_stamina(100.0f)
{
}

Player::~Player()
{
    delete m_pAnimSprite;
    m_pAnimSprite = nullptr;

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

    m_position = Vector2(m_pRenderer->GetWidth() * 0.4f, m_pRenderer->GetHeight() * 0.4f);
    m_bAlive = true;
    return true;
}

void Player::Process(float deltaTime, InputSystem& inputSystem)
{
    HandleMovement(deltaTime, inputSystem);

    ClampToScreen();

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

void Player::HandleMovement(float deltaTime, InputSystem& inputSystem) {
    Vector2 direction(0.0f, 0.0f);

	if (m_noClip == true) { // No up or down movement unless noclipping
        if (IsKeyHeld(inputSystem, SDL_SCANCODE_W) || IsKeyHeld(inputSystem, SDL_SCANCODE_UP))    direction.y -= 1.0f;
        if (IsKeyHeld(inputSystem, SDL_SCANCODE_S) || IsKeyHeld(inputSystem, SDL_SCANCODE_DOWN))  direction.y += 1.0f;
    }

    if (IsKeyHeld(inputSystem, SDL_SCANCODE_A) || IsKeyHeld(inputSystem, SDL_SCANCODE_LEFT))  direction.x -= 1.0f;
    if (IsKeyHeld(inputSystem, SDL_SCANCODE_D) || IsKeyHeld(inputSystem, SDL_SCANCODE_RIGHT)) direction.x += 1.0f;

    //controller input
    if (inputSystem.GetNumberOfControllersAttached() > 0) {
        XboxController* controller = inputSystem.GetController(0);
        if (controller != nullptr && controller->IsConnected())
        {
            Vector2 stick = controller->GetLeftStick();

            //normalise
            const float MAX_AXIS = 32768.0f;
            Vector2 controllerDir(stick.x / MAX_AXIS, stick.y / MAX_AXIS);

            //deadzone handling
            const float DEADZONE = 0.2f;
            if (std::abs(controllerDir.x) > DEADZONE || std::abs(controllerDir.y) > DEADZONE)
            {
                direction = controllerDir;
            }
        }
    }

    //normalising to prevent faster diagonal movement
    if (direction.x != 0.0f || direction.y != 0.0f)
    {
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        direction.x /= length;
        direction.y /= length;
    }

    // Jumping
    if (m_noClip == false) {
        if (m_OnGround && IsKeyHeld(inputSystem, SDL_SCANCODE_SPACE))
        {
            m_Velocity.y = -450.0f;
            m_OnGround = false;
        }

        if (!m_OnGround) {
			// Play jump/fall animation
            if (m_animationState != JUMP)
            {
                m_pAnimSprite = m_pJumpSprite;
				m_pAnimSprite->Restart();
                m_pAnimSprite->Animate();
                m_animationState = JUMP;
            }

            // Apply gravity
            m_Velocity.y += GRAVITY * deltaTime;
        }

        // Apply Vertical Movement
        m_position.y += m_Velocity.y * deltaTime;

        // Reset jump and velocity on ground
        if (m_position.y >= GROUND_Y)
        {
            m_position.y = GROUND_Y;
            m_Velocity.y = 0.0f;
            m_OnGround = true;

            if (m_animationState != IDLE)
            {
                m_pAnimSprite = m_pIdleSprite;
				m_pAnimSprite->Restart();
                m_animationState = IDLE;
            }
        }
        else
        {
            m_OnGround = false;
        }
    }

    //move the player
    m_position += direction * m_speed * deltaTime;

    if (direction.x < 0.0f) {
        m_facingLeft = true;
    }
    else if (direction.x > 0.0f) {
        m_facingLeft = false;
    }
}

void Player::ClampToScreen()
{
    // clamp to screen with halfWidth, to prevent clipping outside screen
    const int screenWidth = m_pRenderer->GetWidth();
    const int screenHeight = m_pRenderer->GetWorldHeight();

    const int spriteHalfWidth = m_pAnimSprite->GetWidth() / 2;
    const int spriteHalfHeight = m_pAnimSprite->GetHeight() / 2;

    float wallMarginX = screenWidth * 0.00f;  // 2% horizontal margin (for the walls)
    float wallMarginY = screenHeight * 0.00f; // PLayer can't move past the top 5%

    float minX = wallMarginX + spriteHalfWidth;
    float maxX = screenWidth - wallMarginX - spriteHalfWidth;
    float minY = wallMarginY + spriteHalfHeight;
    float maxY = static_cast<float>(screenHeight - spriteHalfHeight);


    m_position.x = std::max(minX, std::min(maxX, m_position.x));
    m_position.y = std::max(minY, std::min(maxY, m_position.y));
}

void Player::LoadAnimatedSprites() {
    m_pIdleSprite = m_pRenderer->CreateAnimatedSprite("../assets/player/Idle.png");
    m_pJumpSprite = m_pRenderer->CreateAnimatedSprite("../assets/player/Jump.png");
	m_pMineSprite = m_pRenderer->CreateAnimatedSprite("../assets/player/Swing.png");

    m_pIdleSprite->SetupFrames(32, 32);
    m_pIdleSprite->SetLooping(true);
    m_pIdleSprite->SetFrameDuration(0.1f);
    m_pIdleSprite->Animate();
    m_pIdleSprite->SetScale(1.9f);

    m_pJumpSprite->SetupFrames(32, 32);
    m_pJumpSprite->SetLooping(false);
    m_pJumpSprite->SetFrameDuration(0.1f);
    m_pJumpSprite->Animate();
    m_pJumpSprite->SetScale(1.9f);

	m_pMineSprite->SetupFrames(32, 32);
	m_pMineSprite->SetLooping(true);
	m_pMineSprite->SetFrameDuration(0.1f);
	m_pMineSprite->Animate();
	m_pMineSprite->SetScale(1.9f);

    // Set default
    m_pAnimSprite = m_pIdleSprite;
    m_animationState = IDLE;
}

float Player::GetPlayerHeight(){
    return m_pAnimSprite->GetHeight();
}

bool Player::IsKeyHeld(InputSystem& input, SDL_Scancode key)
{
    return input.GetKeyState(key) == BS_HELD;
}

void Player::SetNoClip(bool noClip)
{
	m_noClip = noClip;
	
    // TODO: TURN OFF TILE COLLISION HERE SOMEHOW
}


