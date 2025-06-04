#include "player.h"
#include "renderer.h"
#include "sprite.h"
#include "animatedsprite.h"
#include "imgui/imgui.h"
#include "gridstate.h"
#include "quadtree.h"

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

    m_position = { 400, 300 };
    //m_position = Vector2(m_pRenderer->GetWidth() * 0.4f, m_pRenderer->GetHeight() * 0.4f);
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

void Player::HandleMovement(float deltaTime, InputSystem& input)
{
    // 1) Read input into direction (X and Y for no-clip; X only if gravity is on).
    Vector2 direction(0.0f, 0.0f);

    if (m_noClip) {
        if (IsKeyHeld(input, SDL_SCANCODE_W) || IsKeyHeld(input, SDL_SCANCODE_UP))    direction.y -= 1.0f;
        if (IsKeyHeld(input, SDL_SCANCODE_S) || IsKeyHeld(input, SDL_SCANCODE_DOWN))  direction.y += 1.0f;
    }
    if (IsKeyHeld(input, SDL_SCANCODE_A) || IsKeyHeld(input, SDL_SCANCODE_LEFT))  direction.x -= 1.0f;
    if (IsKeyHeld(input, SDL_SCANCODE_D) || IsKeyHeld(input, SDL_SCANCODE_RIGHT)) direction.x += 1.0f;

    // Normalize diagonal movement:
    if (direction.x != 0.0f || direction.y != 0.0f) {
        float len = sqrtf(direction.x * direction.x + direction.y * direction.y);
        direction.x /= len;
        direction.y /= len;
    }

    // 2) Jump / Gravity (only if not nop-clip):
    float spriteW = (float)m_pAnimSprite->GetWidth() * m_pAnimSprite->GetScale();
    float spriteH = (float)m_pAnimSprite->GetHeight() * m_pAnimSprite->GetScale();

    if (!m_noClip) {
        // If on ground and space is pressed, give an initial upward velocity:
        if (m_OnGround && IsKeyHeld(input, SDL_SCANCODE_SPACE)) {
            m_Velocity.y = -450.0f;   // jump impulse
            m_OnGround = false;
            // switch to jumping animation...
            if (m_animationState != JUMP) {
                m_pAnimSprite = m_pJumpSprite;
                m_pAnimSprite->Restart();
                m_animationState = JUMP;
            }
        }

        // Only apply gravity if we are not on the ground:
        if (!m_OnGround) {
            m_Velocity.y += GRAVITY * deltaTime;
        }
    }

    // 3) Build the collision box parameters (we’ll reuse for both X and Y):
    //    We want the box to be 70% as wide, 50% as tall, and flush with the bottom of the sprite:
    float boxW = spriteW * 0.7f;
    float boxH = spriteH;
    float boxOffsetX = (spriteW - boxW) * 0.5f;   // center horizontally under sprite
    float boxOffsetY = spriteH - boxH;            // push down so bottom half

    Vector2 offset = GridState::GetInstance().m_gameGrid->GetScreenOffsets();
    float   tileSize = GridState::GetInstance().GetTileSize();

    // BREAKABLE BLOCKS:  
    // If you hold arrow keys, break blocks in that direction:
    if (!m_noClip) {
        if (IsKeyHeld(input, SDL_SCANCODE_UP))    GridState::GetInstance().BreakBlock(m_position, 'U');
        if (IsKeyHeld(input, SDL_SCANCODE_DOWN))  GridState::GetInstance().BreakBlock(m_position, 'D');
        if (IsKeyHeld(input, SDL_SCANCODE_LEFT))  GridState::GetInstance().BreakBlock(m_position, 'L');
        if (IsKeyHeld(input, SDL_SCANCODE_RIGHT)) GridState::GetInstance().BreakBlock(m_position, 'R');
    }

    // CONTROLLER (optional):
    if (input.GetNumberOfControllersAttached() > 0) {
        XboxController* c = input.GetController(0);
        if (c && c->IsConnected()) {
            Vector2 stick = c->GetLeftStick();
            const float MAXAXIS = 32768.0f;
            Vector2 cdir(stick.x / MAXAXIS, stick.y / MAXAXIS);
            const float DEADZONE = 0.2f;
            if (fabsf(cdir.x) > DEADZONE || fabsf(cdir.y) > DEADZONE) {
                direction = cdir;
                // If in no-clip mode, it overrides arrow keys; if gravity mode, we only care about X
            }
        }
    }

    // 4) HORIZONTAL MOVEMENT TEST:
    float desiredX = m_position.x + direction.x * m_speed * deltaTime;
    Box boxX(desiredX + boxOffsetX,
        m_position.y + boxOffsetY,
        boxW,
        boxH);
    if (!GridState::GetInstance().CheckCollision(boxX)) {
        m_position.x = desiredX;
    }
    // face left/right:
    if (direction.x < 0.0f)  m_facingLeft = true;
    else if (direction.x > 0.0f) m_facingLeft = false;

    // 5) VERTICAL MOVEMENT TEST:
    float desiredY = m_position.y + m_Velocity.y * deltaTime;
    Box boxY(m_position.x + boxOffsetX,
        desiredY + boxOffsetY,
        boxW, boxH);

    if (!m_noClip) {
        if (!GridState::GetInstance().CheckCollision(boxY)) {
            m_position.y = desiredY;
            m_OnGround = false;
        }
        else {
            // We hit something vertically (either ground or ceiling).
            // Figure out if this is a ground collision (player falling onto tile):
            float boxBottom = desiredY + boxOffsetY + boxH;                    // world Y where box hits
            int   collidedRow = static_cast<int>(floor((boxBottom - offset.y) / tileSize));
            if (collidedRow < 0) collidedRow = 0;
            // Top of that row in world Y:
            float rowTopY = offset.y + (static_cast<float>(collidedRow) * tileSize);

            // Snap so boxBottom == rowTopY:
            float newY = rowTopY - boxH - boxOffsetY;
            m_position.y = newY;
            m_Velocity.y = 0.0f;
            m_OnGround = true;

            // Switch back to idle animation:
            if (m_animationState != IDLE) {
                m_pAnimSprite = m_pIdleSprite;
                m_pAnimSprite->Restart();
                m_animationState = IDLE;
            }
        }
    }
    else {
        // In no-clip, just move vertically without collision:
        m_position.y = desiredY;
    }

    // 6) CLAMP TO SCREEN (unchanged):
    ClampToScreen();

    // 7) Finally, tell your sprite to animate:
    m_pAnimSprite->Process(deltaTime);
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
    m_pJumpSprite->SetFrameDuration(0.05f);
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


void Player::DrawCollision(float cameraX, float cameraY)
{
    float spriteW = static_cast<float>(m_pAnimSprite->GetWidth()) * m_pAnimSprite->GetScale();
    float spriteH = static_cast<float>(m_pAnimSprite->GetHeight()) * m_pAnimSprite->GetScale();

    float boxW = spriteW * 0.7f;
    float boxH = 10.0f;

    float boxOffsetX = (spriteW - boxW) * 0.5f;
    float boxOffsetY = spriteH - boxH;
    float worldRectX = m_position.x + boxOffsetX;
    float worldRectY = m_position.y + boxOffsetY;

    float screenRectX = worldRectX - cameraX;
    float screenRectY = worldRectY - cameraY;

    ImVec2 topLeft(
        screenRectX,
        screenRectY
    );
    ImVec2 bottomRight(
        screenRectX + boxW,
        screenRectY + boxH
    );

    ImU32 col = IM_COL32(255, 0, 0, 255);
    ImGui::GetForegroundDrawList()->AddRect(topLeft, bottomRight, col);
}