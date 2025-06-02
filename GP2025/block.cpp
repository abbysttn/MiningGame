#include "block.h"

#include "renderer.h"
#include "animatedsprite.h"

#include <cstdlib>
#include "inlinehelper.h"
#include "logmanager.h"

#include <string>

Block::Block() : m_sprite(0), m_isBroken(false) {}

Block::~Block()
{
	delete m_sprite;
	m_sprite = nullptr;
}

bool Block::Initialise(Renderer& renderer, int depth)
{
	m_depth = depth;
	GetBlockType(m_depth, m_filepath);

	m_sprite = renderer.CreateAnimatedSprite(m_filepath);
	m_sprite->SetupFrames(8, 8);
	m_sprite->SetLooping(false);
	m_sprite->SetFrameDuration(m_animatingTime);

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
	if (m_active && !m_isBroken) {
		m_sprite->SetX(static_cast<int>(m_position.x));
		m_sprite->SetY(static_cast<int>(m_position.y));

		if (m_isBreaking) {
			if (m_isBreaking && m_currentTime == 0.0f) {
				m_currentBlockStatus++;
				m_sprite->SetCurrentFrame(m_currentBlockStatus);
			}

			m_currentTime += deltaTime;

			if (m_currentTime >= m_animatingTime) {
				m_isBreaking = false;
				m_currentTime = 0.0f;
			}
		}		
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

int Block::GetSpriteHeight()
{
	return m_sprite->GetHeight();
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

void Block::BreakBlock()
{
	if (!m_isBroken && !m_isBreaking) {
		m_isBreaking = true;

		if (m_sprite->FramesFinished()) {
			m_isBroken = true;
			m_active = false;
		}
	}
}

void Block::GetBlockType(int& depth, const char*& filepath)
{
	float percentage = 0.0f;
	bool IsGem = false;

	if (depth < 40) {
		percentage = 0.00f;
	}
	else if (depth < 50 && depth > 40) {
		percentage = 0.05f;
	}
	else if (depth < 70 && depth > 50) {
		percentage = 0.15f;
	}
	else if (depth >= 71) {
		percentage = 0.3f;
	}

	float chance = GetRandomPercentage();

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
