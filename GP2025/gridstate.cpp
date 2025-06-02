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

void GridState::BreakBlock(Block* block)
{
	block->SetActive(false);
}

void GridState::StopBreakingBlock(Block* block)
{
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
				Box blockBox(block->Position().x - block->GetSpriteWidth() / 2.0f, block->Position().y - block->GetSpriteWidth() / 2.0f, (float)block->GetSpriteWidth(),
					(float)block->GetSpriteHeight());

				if (CollisionHelper::IsColliding(box, blockBox)) {
					LogManager::GetInstance().Log("Colliding");

					return true;
				}
			}
		}
	}

	return false;
}
