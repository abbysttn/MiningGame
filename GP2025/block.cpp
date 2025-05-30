#include "block.h"

#include "renderer.h"
#include "sprite.h"

#include <cstdlib>
#include "inlinehelper.h"
#include "logmanager.h"

#include <string>

Block::Block() : m_sprite(0) {}

Block::~Block()
{
	delete m_sprite;
	m_sprite = nullptr;
}

bool Block::Initialise(Renderer& renderer, int depth)
{
	m_depth = depth;
	GetBlockType(m_depth, m_filepath);

	m_sprite = renderer.CreateSprite(m_filepath);

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

void Block::GetBlockType(int& depth, const char*& filepath)
{
	float percentage = 0.0f;
	bool IsGem = false;

	if (depth < 40) {
		percentage = 0.00f;
	}
	else if (depth < 50) {
		percentage = 0.05f;
	}
	else if (depth < 70) {
		percentage = 0.15f;
	}
	else if (depth >= 71) {
		percentage = 0.3f;
	}

	float chance = GetRandomPercentage();
	LogManager::GetInstance().Log(std::to_string(chance).c_str());

	if (chance <= percentage) {
		IsGem = true;
	}

	if (IsGem) {
		filepath = "../assets/gem.png";
	}
	else {
		filepath = "../assets/dirt.png";
	}
}
