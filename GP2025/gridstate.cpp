#include "gridstate.h"

#include "gameobjectpool.h"
#include "quadtree.h"
#include "Renderer.h"

#include "collisionhelper.h"

#include "logmanager.h"

void GridState::CreateGrid(Renderer& renderer)
{
	m_gameGrid = new Grid();
	m_gameGrid->Initialise(renderer);
}

GameObjectPool* GridState::GetPool()
{
	return nullptr;
}

void GridState::BreakBlock(Vector2 position, char direction)
{
	Vector2 gridCoords;
	float blockWidth = m_gameGrid->GetBlockSize().x;
	float blockHeight = m_gameGrid->GetBlockSize().y;

	int x = static_cast<int>((position.x - m_gameGrid->GetScreenOffsets().x + (blockWidth / 2)) / blockWidth);
	int y = static_cast<int>((position.y - m_gameGrid->GetScreenOffsets().y + (blockHeight / 2)) / blockHeight);

	int targetX = x;
	int targetY = y;

	switch (direction) {
	case 'L': targetX--; break;
	case 'R': targetX++; break;
	case 'U': targetY--; break;
	case 'D': targetY++; break;
	}

	//fixes issue of first row not breaking
	if (direction == 'D' && y == 0) {
		targetY -= 2;
	}

	if (direction == 'U' && y <= 0) {
		return;
	}

	targetX = max(0, min(targetX, m_gameGrid->GetColumns() - 1));
	targetY = max(0, min(targetY, m_gameGrid->GetRows() - 1));

	gridCoords = { static_cast<float>(targetX), static_cast<float>(targetY) };

	Block* block = m_gameGrid->GetBlockFromGrid(gridCoords);

	if (block != nullptr) {
		block->BreakBlock();
	}
}

void GridState::ResetGrid()
{
	delete m_gameGrid;
	m_gameGrid = nullptr;
}

char GridState::GetBlockType(Block* block)
{
	return 0;
}

void GridState::ProcessGrid(float deltaTime, InputSystem& inputSystem)
{
	m_gameGrid->Process(deltaTime, inputSystem);
}

void GridState::DrawGrid(Renderer& renderer)
{
	m_gameGrid->Draw(renderer);
}

bool GridState::CheckCollision(Box& box)
{
	QuadTree& tree = m_gameGrid->GetCollisionTree();

	auto potentialCollision = tree.queryRange(box);

	for (auto* object : potentialCollision) {
		if (Block* block = dynamic_cast<Block*>(object)) {
			if (block->IsActive()) {
				Box blockBox(block->Position().x, block->Position().y, (float)block->GetSpriteWidth(),
					(float)block->GetSpriteHeight());

				if (CollisionHelper::IsColliding(blockBox, box)) {
					return true;
				}
			}
		}
	}

	return false;
}
