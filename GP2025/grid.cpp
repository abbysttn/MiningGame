#include "grid.h"

#include "gameobjectpool.h"
#include "quadtree.h"
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

	m_tileSize = (screenWidth / ((float)m_cols));

	float levelPixelWidth = m_cols * m_tileSize;
	float levelPixelHeight = m_rows * m_tileSize;

	screenOffsetX = ((screenWidth - levelPixelWidth) / 2.0f) + (m_tileSize / 2.0f);
	screenOffsetY = (m_backgroundHeight / 7.05f);

	if (m_isRegular) {

		for (size_t y = 0; y < (size_t)m_rows; y++) {
			for (size_t x = 0; x < (size_t)m_cols; x++) {
				if (!InitObjects(renderer, x, y)) {
					return false;
				}
			}
		}

	}
	else {
		for (size_t y = 0; y < 15; y++) {
			for (size_t x = 0; x < (size_t)m_cols; x++) {
				if (!InitCutsceneObjects(renderer, x, y)) {
					return false;
				}
			}
		}
	}

	float fullScreenHeight = (float)renderer.GetHeight() + (m_rows * m_tileSize);

	m_collisionTree = make_unique<QuadTree>(Box(0.0f, 0.0f, (float)renderer.GetWidth(), fullScreenHeight));

	return true;
}

void Grid::Process(float deltaTime, InputSystem& inputSystem)
{
	m_collisionTree->clear();

	for (size_t i = 0; i < m_grid->totalCount(); i++) {
		if (GameObject* obj = m_grid->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<Block*>(obj)) {
				Block* block = dynamic_cast<Block*>(obj);
				if (block->IsActive()) {
					block->Process(deltaTime);

					if (block && block->IsActive()) {
						float blockSize = (float)block->GetSpriteWidth();
						Box blockRange(
							block->Position().x,
							block->Position().y,
							blockSize,
							blockSize
						);

						m_collisionTree->insert(block, blockRange);
					}
				}
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

QuadTree& Grid::GetCollisionTree()
{
	return *m_collisionTree;
}

Vector2 Grid::GetBlockSize()
{
	return m_blockSize;
}

Block* Grid::GetBlockFromGrid(const Vector2 position) const
{
	int spriteIndex = static_cast<int>(position.y * m_cols + position.x);
	Block* closestBlock = nullptr;

	if (GameObject* obj = m_grid->getObjectAtIndex(spriteIndex)) {
		if (obj && dynamic_cast<Block*>(obj)) {
			Block* block = dynamic_cast<Block*>(obj);

			closestBlock = block;
		}
	}

	return closestBlock;
}

int Grid::GetRows()
{
	return (size_t)m_rows;
}

int Grid::GetColumns()
{
	return (size_t)m_cols;
}

Vector2 Grid::GetScreenOffsets()
{
	return Vector2(screenOffsetX, screenOffsetY);
}

void Grid::SetBackgroundHeight(float height)
{
	m_backgroundHeight = height;
}

void Grid::SetGridType(bool regular)
{
	m_isRegular = regular;
}

Vector2 Grid::GetPlayerStartPos()
{
	return m_playerStartPos;
}

bool Grid::InitObjects(Renderer& renderer, size_t x, size_t y)
{
	int spriteIndex = y * m_cols + x;

	if (GameObject* obj = m_grid->getObject()) {
		Block* block = dynamic_cast<Block*>(obj);

		block->Initialise(renderer, y, x);

		if (!block) {
			m_grid->release(block);
			return false;
		}

		float spriteWidth = (float)block->GetSpriteWidth();

		float scaleFactor = m_tileSize / spriteWidth;

		block->SetScale(scaleFactor);

		block->Position().x = (x * m_tileSize) + screenOffsetX;
		block->Position().y = (y * m_tileSize) + screenOffsetY;

		m_blockSize.x = static_cast<float>(block->GetSpriteWidth());
		m_blockSize.y = static_cast<float>(block->GetSpriteHeight());

		if (y == 0 && (x <= 4 || x >= 15)) {
			block->SetBreakable(false);
		}

		if (x == 0 || x == 19) {
			block->SetBreakable(false);
		}

		return true;
	}

	return false;
}

bool Grid::InitCutsceneObjects(Renderer& renderer, size_t x, size_t y)
{
	int spriteIndex = y * m_cols + x;

	if (GameObject* obj = m_grid->getObject()) {
		Block* block = dynamic_cast<Block*>(obj);

		block->SetCutsceneBlock(true);
		block->Initialise(renderer, y, x);

		if (!block) {
			m_grid->release(block);
			return false;
		}

		float spriteWidth = (float)block->GetSpriteWidth();

		float scaleFactor = m_tileSize / spriteWidth;

		block->SetScale(scaleFactor);

		block->Position().x = (x * m_tileSize) + screenOffsetX;
		block->Position().y = (y * m_tileSize) + screenOffsetY;

		m_blockSize.x = static_cast<float>(block->GetSpriteWidth());
		m_blockSize.y = static_cast<float>(block->GetSpriteHeight());

		if (y >= 5 && y <= 7) {
			if (x >= 5) block->SetActive(false);
			if (x == 5) m_playerStartPos = block->Position();
		}

		return true;
	}

	return false;
}
