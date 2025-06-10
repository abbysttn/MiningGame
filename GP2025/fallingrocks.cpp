#include "fallingrocks.h"

#include "renderer.h"
#include "inputsystem.h"

#include "inlinehelper.h"

#include "block.h"

FallingRocks::FallingRocks() : m_blocks(nullptr) {}

FallingRocks::~FallingRocks()
{
	delete m_blocks;
	m_blocks = nullptr;
}

bool FallingRocks::Initialise(Renderer& renderer)
{
	m_blocks = new GameObjectPool(Block(), 20);

	for (size_t i = 0; i < m_blocks->totalCount(); i++) {
		if (GameObject* obj = m_blocks->getObjectAtIndex(i)) {
			Block* block = dynamic_cast<Block*>(obj);

			if (!block) {
				m_blocks->release(block);
				return false;
			}

			block->SetCutsceneBlock(true);

			block->Initialise(renderer, GetRandom(10, 20), 1);

			float scaleFactor = m_scale / block->GetSpriteWidth();

			block->SetScale(scaleFactor);

			block->Position() = m_startPos;

			float direction = (((i + 1) * 1.5f) / 10.0f) - 1.5f;

			block->SetXDirection(direction);
		}
	}

	return true;
}

void FallingRocks::Process(float deltaTime, InputSystem& inputSystem)
{
	if (!m_falling) {
		for (size_t i = 0; i < m_blocks->totalCount(); i++) {
			if (GameObject* obj = m_blocks->getObjectAtIndex(i)) {
				if (obj && dynamic_cast<Block*>(obj)) {
					Block* block = dynamic_cast<Block*>(obj);
					if (block->IsActive()) {
						block->Process(deltaTime);
					}
				}
			}
		}
	}
	else {

		m_fallSpeed += m_gravity * deltaTime;

		for (size_t i = 0; i < m_blocks->totalCount(); i++) {
			if (GameObject* obj = m_blocks->getObjectAtIndex(i)) {
				if (obj && dynamic_cast<Block*>(obj)) {
					Block* block = dynamic_cast<Block*>(obj);
					if (block->IsActive()) {

						Vector2 pos = block->Position();

						if (i <= 8) pos.y += (m_fallSpeed * deltaTime) - (block->GetXDirection() / 2.0f);
						if (i >= 12) pos.y += (m_fallSpeed * deltaTime) + (block->GetXDirection() / 2.0f);
						if (i == 9 || i == 10 || i == 11) pos.y += m_fallSpeed * deltaTime;

						pos.x += block->GetXDirection();

						if (pos.y >= (m_endPos.y - (block->GetXDirection() / 2.0f))) {
							pos.y = m_endPos.y;
							m_falling = false;
							m_fallSpeed = 0.0f;
						}

						block->Position() = pos;
						block->Process(deltaTime);
					}
				}
			}
		}
	}
}

void FallingRocks::Draw(Renderer& renderer)
{
	for (size_t i = 0; i < m_blocks->totalCount(); i++) {
		if (GameObject* obj = m_blocks->getObjectAtIndex(i)) {
			if (obj && obj->IsActive()) {
				Block* block = dynamic_cast<Block*>(obj);
				block->Draw(renderer);
			}
		}
	}
}

void FallingRocks::DebugDraw()
{
}

void FallingRocks::SetScale(float scale)
{
	m_scale = scale;
}

void FallingRocks::SetStartPos(Vector2 pos)
{
	m_startPos = pos;
}

void FallingRocks::SetEndPos(Vector2 pos)
{
	m_endPos = pos;
}

void FallingRocks::SetFalling(bool falling)
{
	m_falling = falling;
}

bool FallingRocks::GetStatus()
{
	return false;
}
