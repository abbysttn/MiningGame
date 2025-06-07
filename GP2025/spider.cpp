#include "spider.h"

#include "renderer.h"
#include "animatedsprite.h"

#include "logmanager.h"

#include <cstdlib>

Spider::Spider() : m_sprite(0), m_alive(false) {}

Spider::~Spider()
{
	delete m_sprite;
	m_sprite = nullptr;
}

bool Spider::Initialise(Renderer& renderer, const char* filepath)
{
	m_sprite = renderer.CreateAnimatedSprite(filepath);
	m_sprite->SetScale(4.0f);
	m_sprite->SetupFrames(14, 12);
	m_sprite->SetLooping(true);
	m_sprite->SetFrameDuration(0.2f);
	m_sprite->Animate();

	if (!m_sprite) {
		LogManager::GetInstance().Log("Failed to load spider.");
	}

	m_position.x = 500;
	m_position.y = 300;
	return true;
}

void Spider::Process(float deltaTime)
{
	if (m_alive) {
		UpdatePushback(deltaTime);

		m_sprite->SetX(static_cast<int>(m_position.x));
		m_sprite->SetY(static_cast<int>(m_position.y));
		m_sprite->Process(deltaTime);
	}
}

void Spider::Draw(Renderer& renderer, bool facingLeft)
{
	m_sprite->Draw(renderer, facingLeft);
}

Vector2& Spider::Position()
{
	return m_position;
}

void Spider::SetColour(float red, float green, float blue)
{
	m_sprite->SetRedTint(red);
	m_sprite->SetGreenTint(green);
	m_sprite->SetBlueTint(blue);
}

int Spider::GetSpriteHeight()
{
	return m_sprite->GetHeight();
}

int Spider::GetSpriteWidth() const
{
	return m_sprite->GetWidth();
}

GameObject* Spider::Create() const
{
	return new Spider();
}

bool Spider::IsActive() const
{
	return m_alive;
}

void Spider::Reset()
{
	m_position = { 0, 0 };
}

void Spider::SetScale(float scale)
{
	m_sprite->SetScale(scale);
}

void Spider::SetAlive(bool alive)
{
	m_alive = alive;
}

Vector2 Spider::Attack(Vector2 direction, float deltaTime)
{
	Vector2 testPos = m_position;

	int currentFrame = m_sprite->GetCurrentFrame();

	if (currentFrame >= 3 && currentFrame <= 5) {
		testPos += direction * 50.0f * deltaTime;
	}

	return testPos;
}

void Spider::ApplyPushback(Vector2 direction)
{
	float length = sqrtf(direction.x * direction.x + direction.y * direction.y);

	if (length != 0) {
		direction.x /= length;
		direction.y /= length;
	}

	m_pushbackVelocity = direction.x * 600.0f;
	m_yPushback = 1000.0f;
	m_isPushed = true;
}

void Spider::UpdatePushback(float deltaTime)
{
	if (m_isPushed) {
		m_position.x += m_pushbackVelocity * deltaTime;
		m_position.y -= m_yPushback * deltaTime;

		m_pushbackVelocity *= 0.9f;
		m_yPushback *= 0.9f;

		if (fabs(m_pushbackVelocity) < 0.01f) {
			m_pushbackVelocity = 0;
			m_yPushback = 0;
			m_isPushed = false;
		}
	}
}

bool Spider::IsPushed()
{
	return m_isPushed;
}

bool Spider::IsDead()
{
	if (m_sprite->GetCurrentFrame() >= 8) {
		return true;
	}

	return false;
}
