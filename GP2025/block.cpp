#include "block.h"

#include "renderer.h"
#include "animatedsprite.h"

#include <cstdlib>
#include "inlinehelper.h"
#include "logmanager.h"

#include <string>
#include <cmath>

Block::Block() : m_sprite(0), m_isBroken(false) {}

Block::~Block()
{
	delete m_sprite;
	m_sprite = nullptr;
}

bool Block::Initialise(Renderer& renderer, int depth, int x)
{
	m_depth = depth;
	GetBlockType(m_depth, m_filepath, x);

	m_sprite = renderer.CreateAnimatedSprite(m_filepath);

	if (m_blockType == 'O') {
		m_sprite->SetRedTint(0.1f);
		m_sprite->SetGreenTint(0.3f);
		m_sprite->SetBlueTint(1.0f);
	}

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
	if (m_active) {
		m_sprite->SetX(static_cast<int>(m_position.x));
		m_sprite->SetY(static_cast<int>(m_position.y));

		if (m_isBreaking) {
			if (m_isBreaking && m_currentTime == 0.0f) {
				m_currentBlockStatus++;
				m_sprite->SetCurrentFrame(m_currentBlockStatus);
				if (m_sprite->FramesFinished()) {
					m_isBroken = true;
					m_active = false;
				}
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
	m_sprite->Draw(renderer, false);
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

void Block::SetBreakable(bool canBreak)
{
	m_canBreak = canBreak;
}

bool Block::CanBreak()
{
	return m_canBreak;
}

void Block::BreakBlock(bool instantMine)
{
	if (m_isBroken || m_isBreaking) {
		return;
	}

	if (instantMine) {
		m_isBroken = true;
		m_active = false;
		return;
	}
	else {
		m_isBreaking = true;
	}
}

bool Block::BlockBroken()
{
	return m_isBroken;
}

char Block::GetBlockType()
{
	return m_blockType;
}

int Block::GetResourceAmount()
{
	if (!m_resourceGiven) {
		m_resourceGiven = true;
		if (m_depth == 0) {
			return 1;
		}
		else {
			return static_cast<int>(log2(m_depth)) + 1;
		}
	}
	else {
		return 0;
	}
}

bool Block::ResourcesGiven()
{
	return m_resourceGiven;
}

bool Block::IsHazard()
{
	return m_isHazard;
}

void Block::GetBlockType(int& depth, const char*& filepath, int x)
{
	if (depth <= 0 && (x <= 4 || x >= 15)) {
		filepath = "../assets/brock.png";
		return;
	}

	if (x == 0 || x == 19) {
		filepath = "../assets/brock.png";
		return;
	}

	float percentage = 0.0f;
	bool IsGem = false;

	if (depth < 5) {
		filepath = "../assets/dirt.png";
		m_blockType = 'D';
		return;
	}
	else if (depth < 15 && depth > 5) {
		percentage = 0.02f;
	}
	else if (depth < 20 && depth > 15) {
		percentage = 0.08f;
	}
	else if (depth >= 20) {
		percentage = 0.1f;
	}

	// 5% Oxygen Chance. Runs before gem chance.
	if (depth >= 5 && GetRandomPercentage() < 0.05f) {
		filepath = "../assets/stone.png";
		m_blockType = 'O';
		return;
	}

	float chance = GetRandomPercentage();


	if (chance <= percentage) {
		IsGem = true;
	}

	if (IsGem) {
		filepath = "../assets/gem.png";
		m_blockType = 'G';
	}
	else {
		float dirtChance = 0.8f * exp(-0.01f * m_depth);
		float roll = GetRandomPercentage();

		if (dirtChance <= 0.0f) {
			dirtChance = 0.0f;
		}

		if (roll < dirtChance) {
			filepath = "../assets/dirt.png";
			m_blockType = 'D';
			return;
		}
		else {
			float hazardChance = 0.05f * exp(-0.01f * m_depth);
			roll = GetRandomPercentage();

			if (roll < hazardChance) {
				m_isHazard = true;
				m_canBreak = false;

				int hazard = GetRandom(1, 2);

				if (hazard == 1) {
					filepath = "../assets/lava.png";
					return;
				}
				else {
					filepath = "../assets/bomb.png";
					return;
				}
			}
			else {
				filepath = "../assets/stone.png";
				m_blockType = 'S';
				return;
			}
		}
	}
}
