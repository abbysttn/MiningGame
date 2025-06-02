#include "grid.h"

#include "gameobjectpool.h"
#include "block.h"

#include "renderer.h"
#include "sprite.h"

Grid::Grid() : m_tileSize(48.0f), m_grid(nullptr) {}

Grid::~Grid()
{
	delete m_grid;
	m_grid = nullptr;
}

bool Grid::Initialise(Renderer& renderer)
{
	m_grid = new GameObjectPool(Block(), m_count);

	float screenWidth = (float)renderer.GetWidth();
	float screenHeight = (float)renderer.GetHeight();

	m_tileSize = (screenWidth / ((float)m_cols + 1)) - 2.5f;

	float levelPixelWidth = m_cols * m_tileSize;
	float levelPixelHeight = m_rows * m_tileSize;

	screenOffsetX = ((screenWidth - levelPixelWidth) / 2.0f) + (m_tileSize / 2.0f);
	screenOffsetY = screenHeight*0.67f + (m_tileSize / 2.0f);

	for (int y = 0; y < m_rows; y++) {
		for (int x = 0; x < m_cols; x++) {
			if (!InitObjects(renderer, x, y)) {
				return false;
			}
		}
	}

	return true;
}

void Grid::Process(float deltaTime, InputSystem& inputSystem)
{
	for (size_t i = 0; i < m_grid->totalCount(); i++) {
		if (GameObject* obj = m_grid->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<Block*>(obj)) {
				Block* block = dynamic_cast<Block*>(obj);
				block->Process(deltaTime);
			}
		}
	}
}

void Grid::Draw(Renderer& renderer)
{
	for (size_t i = 0; i < m_grid->totalCount(); i++) {
		if (GameObject* obj = m_grid->getObjectAtIndex(i)) {
			if (obj && obj->IsActive()) {
				Block* block = dynamic_cast<Block*>(obj);
				block->Draw(renderer);
			}
		}
	}
}

void Grid::DebugDraw()
{
}

bool Grid::InitObjects(Renderer& renderer, size_t x, size_t y)
{
	int spriteIndex = y * m_cols + x;

	if (GameObject* obj = m_grid->getObject()) {
		Block* block = dynamic_cast<Block*>(obj);

		block->Initialise(renderer, y);

		if (!block) {
			m_grid->release(block);
			return false;
		}

		float spriteWidth = (float)block->GetSpriteWidth();

		float scaleFactor = m_tileSize / spriteWidth;

		block->SetScale(scaleFactor);

		block->Position().x = (x * m_tileSize) + screenOffsetX;
		block->Position().y = (y * m_tileSize) + screenOffsetY;	

		return true;
	}

	return false;
}
