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
	m_position.y = 550;
	return true;
}

void Spider::Process(float deltaTime)
{
	if (m_alive) {
		m_sprite->SetX(static_cast<int>(m_position.x));
		m_sprite->SetY(static_cast<int>(m_position.y));
		m_sprite->Process(deltaTime);
	}
}

void Spider::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
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
