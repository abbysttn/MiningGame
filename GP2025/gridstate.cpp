#include "gridstate.h"
#include "player.h"

#include "gameobjectpool.h"
#include "quadtree.h"
#include "Renderer.h"
#include "player.h"

#include "collisionhelper.h"

#include "logmanager.h"

#include <string>
#include <cmath>
#include "inlinehelper.h"

void GridState::CreateGrid(Renderer& renderer, float backgroundScale)
{
	m_gameGrid = new Grid();
	m_gameGrid->SetBackgroundHeight(backgroundScale);
	m_gameGrid->Initialise(renderer);
}

GameObjectPool* GridState::GetPool()
{
	return nullptr;
}

void GridState::BreakBlock(Vector2 position, char direction, Player* player)
{
	Vector2 gridCoords;
	float blockWidth = m_gameGrid->GetBlockSize().x;
	float blockHeight = m_gameGrid->GetBlockSize().y;

	int x = static_cast<int>((position.x - m_gameGrid->GetScreenOffsets().x + (blockWidth / 2)) / blockWidth);
	int y = static_cast<int>((position.y - m_gameGrid->GetScreenOffsets().y + (blockHeight / 2)) / blockHeight);

	if (y == 0) {
		if (position.y >= m_gameGrid->GetScreenOffsets().y) {
			y = 0;
		}
		else {
			y = -1;
		}
	}

	int targetX = x;
	int targetY = y;

	switch (direction) {
	case 'L': targetX--; break;
	case 'R': targetX++; break;
	case 'U': targetY--; break;
	case 'D': targetY++; break;
	}

	if (targetY < 0) {
		return;
	}

	targetX = max(0, min(targetX, m_gameGrid->GetColumns() - 1));
	targetY = max(0, min(targetY, m_gameGrid->GetRows() - 1));

	gridCoords = { static_cast<float>(targetX), static_cast<float>(targetY) };

	Block* block = m_gameGrid->GetBlockFromGrid(gridCoords);

	if (block != nullptr) {
		if (block->CanBreak()) {
			block->BreakBlock(player->IsInstantMine());

			m_digBlock = true;

			if (block->ResourcesGiven()) {
				return;
			}

			if (block->BlockBroken()) {
				char blockType = block->GetBlockType();
				int resourceAmount = block->GetResourceAmount();

				m_breakBlock = true;

				ResourceType type;
				bool recognizedType = true;

				switch (blockType) 
				{

				case 'G': 
					type = ResourceType::GEM; 
					m_lastBlockType = 2; 
					m_gemCount += resourceAmount;
					break;
				case 'D': 
					type = ResourceType::DIRT;
					m_lastBlockType = 0; 
					m_dirtCount += resourceAmount;
					break;
				case 'S': 
					type = ResourceType::STONE; 
					m_lastBlockType = 1; 
					m_stoneCount += resourceAmount;
					break;
				case 'O': 
					if (player != nullptr) 
					{
						player->AddOxygen(5.0f);
					}
					m_lastBlockType = 3;
					recognizedType = false;
					break;

				default: 
					recognizedType = false; 
					break;
				}

				if (recognizedType && resourceAmount > 0 && player != nullptr)
				{
					player->AddResource(type, resourceAmount);
				}

				//blockbreak particle activate
				m_blockBroken = true;
				m_lastBrokenPos = m_brokenBlockPos;
				m_brokenBlockPos = block->Position();
				m_brokenBlockTile = targetY;
			}
		}
	}
}

void GridState::ResetGrid()
{
	delete m_gameGrid;
	m_gameGrid = nullptr;

	m_dirtCount = 0;
	m_gemCount = 0;
	m_stoneCount = 0;
}

void GridState::ProcessGrid(float deltaTime, InputSystem& inputSystem)
{
	if (update1) {
		m_blockBroken = false;
		update1 = false;
	}
	else {
		update1 = true;
	}

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

					m_touchingHazard = block->IsHazard();

					return true;
				}
			}
		}
	}

	return false;
}

bool GridState::CheckHazards()
{
	return m_touchingHazard;
}

float GridState::GetTileSize()
{
	return m_gameGrid->GetTileSize();
}

bool GridState::CheckBlockDig() {
	if (m_digBlock == true) {
		m_digBlock = false;
		return true;
	}
	else {
		return false;
	}
}

int GridState::GetLastBlockType() {
	return m_lastBlockType;
}

bool GridState::CheckBlockBreak() {
	if (m_breakBlock == true) {
		m_breakBlock = false;
		return true;
	}
	else {
		return false;
	}
}
Vector2 GridState::GetBrokenBlockPos()
{
	m_blockBroken = false;
	return m_brokenBlockPos;
}

bool GridState::IsBlockBroken()
{
	return m_blockBroken;
}

bool GridState::SpiderSpawn()
{
	if (m_brokenBlockTile < 1) return false;

	float spawnChance = 0.05f;

	return (GetRandomPercentage() < spawnChance);
}

Vector2 GridState::GetBlockSize()
{
	return m_gameGrid->GetBlockSize();
}
