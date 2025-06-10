#include "fallingrocks.h"

#include "renderer.h"
#include "inputsystem.h"

#include "inlinehelper.h"
#include <cmath>

#include "block.h"

FallingRocks::FallingRocks() : m_blocks(nullptr) {}

FallingRocks::~FallingRocks()
{
	delete m_blocks;
	m_blocks = nullptr;
}

bool FallingRocks::Initialise(Renderer& renderer)
{
	m_blocks = new GameObjectPool(Block(), 30);

	m_fallDelays.resize(m_blocks->totalCount(), 0.0f);

	for (size_t i = 0; i < m_blocks->totalCount(); i++) {
		if (GameObject* obj = m_blocks->getObjectAtIndex(i)) {
			Block* block = dynamic_cast<Block*>(obj);

			if (!block) {
				m_blocks->release(block);
				return false;
			}

			block->SetCutsceneBlock(true);

			block->Initialise(renderer, GetRandom(10, 30), 1);

			float scaleFactor = m_scale / block->GetSpriteWidth();

			block->SetScale(scaleFactor);

			block->Position() = m_startPos;
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
		const float delayBetweenBlocks = 0.05f;

		m_fallSpeed += m_gravity * deltaTime;
		bool allLanded = true;

		const float baseHeight = m_endPos.y;
		const int layers = 5;
		const float layerHeight = 35.0f;
		const int blocksPerLayer = m_blocks->totalCount() / layers;

		for (size_t i = 0; i < m_blocks->totalCount(); i++) {
			if (GameObject* obj = m_blocks->getObjectAtIndex(i)) {
				if (obj && dynamic_cast<Block*>(obj)) {
					Block* block = dynamic_cast<Block*>(obj);
					if (block->IsActive()) {

						m_fallDelays[i] += deltaTime;

						if (m_fallDelays[i] < delayBetweenBlocks * i)
							continue;

						Vector2 pos = block->Position();

						int layer = i / blocksPerLayer;
						if (layer >= layers) layer = layers - 1;

						float centerIndex = blocksPerLayer / 2.0f;
						float distanceFromCenter = (i % blocksPerLayer) - centerIndex;

						float xSpread = 100.0f * (1.0f - (float)layer / layers);
						float targetX = m_startPos.x + (distanceFromCenter * xSpread / centerIndex);

						float targetY = baseHeight - (layer * layerHeight);

						if (pos.y < targetY) {
							allLanded = false;
							pos.y += m_fallSpeed * deltaTime;
							pos.x += (targetX - pos.x) * deltaTime;
						}
						else {
							pos = { targetX, targetY };
						}

						block->Position() = pos;
						block->Process(deltaTime);
					}
				}
			}
		}

		if (allLanded) {
			m_falling = false;
			m_fallSpeed = 0.0f;
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
