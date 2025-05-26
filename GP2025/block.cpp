#include "block.h"

#include "renderer.h"
#include "sprite.h"

#include <cstdlib>
#include "logmanager.h"

Block::Block() : m_sprite(0) {}

Block::~Block()
{
	delete m_sprite;
	m_sprite = nullptr;
}

bool Block::Initialise(Renderer& renderer)
{
	m_sprite = renderer.CreateSprite("../assets/dirt.png");

	if (!m_sprite) {
		LogManager::GetInstance().Log("Failed to load block.");
	}

	m_active = true;

	m_position.x = 0;
	m_position.y = 0;
	return true;
}

void Block::Process(float deltaTime)
{
	if (m_active) {
		m_sprite->SetX(static_cast<int>(m_position.x));
		m_sprite->SetY(static_cast<int>(m_position.y));
	}
	m_sprite->Process(deltaTime);
}

void Block::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
}

Vector2& Block::Position()
{
	return m_position;
}

void Block::SetColour(float red, float green, float blue)
{
	m_sprite->SetRedTint(red);
	m_sprite->SetGreenTint(green);
	m_sprite->SetBlueTint(blue);
}

int Block::GetSpriteWidth() const
{
	return m_sprite->GetWidth();
}

GameObject* Block::Create() const
{
	return new Block();
}

bool Block::IsActive() const
{
	return m_active;
}

void Block::Reset()
{
	m_position = {0, 0};
}

void Block::SetScale(float scale)
{
	m_sprite->SetScale(scale);
}

void Block::SetActive(bool active)
{
	m_active = active;
}
