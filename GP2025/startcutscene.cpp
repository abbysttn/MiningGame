#include "startcutscene.h"

#include "renderer.h"
#include "inputsystem.h"

#include "grid.h"
#include "block.h"
#include "game.h"
#include "cutsceneplayer.h"

#include "fallingrocks.h"

#include "logmanager.h"

StartCutscene::StartCutscene() : m_grid(nullptr), m_player(nullptr), m_rocks(nullptr) {}

StartCutscene::~StartCutscene()
{
	delete m_grid;
	m_grid = nullptr;

	delete m_player;
	m_player = nullptr;

	delete m_rocks;
	m_rocks = nullptr;
}

bool StartCutscene::Initialise(Renderer& renderer)
{
	m_grid = new Grid();
	m_grid->SetBackgroundHeight(renderer.GetHeight() - (m_grid->GetTileSize() * 20.0f));
	m_grid->SetGridType(false);
	m_grid->Initialise(renderer);

	m_player = new CutscenePlayer();
	m_player->Initialise(renderer);
	m_player->SetScale(m_grid->GetTileSize() / 35.0f);
	m_player->Position() = m_grid->GetPlayerStartPos();

	m_player->SetState(MINE);
	m_player->SetFlip(true);

	m_rocks = new FallingRocks();
	m_rocks->SetStartPos(m_grid->GetRockStartPos());
	m_rocks->SetEndPos(m_grid->GetRockEndPos());
	m_rocks->SetScale(m_grid->GetTileSize());
	m_rocks->Initialise(renderer);

	return true;
}

void StartCutscene::Process(float deltaTime, InputSystem& inputSystem)
{
	m_grid->Process(deltaTime, inputSystem);

	m_rocks->Process(deltaTime, inputSystem);

	m_player->Process(deltaTime);

	Vector2 gridCoords = { 4, 7 };

	Block* block = m_grid->GetBlockFromGrid(gridCoords);

	if (block != nullptr) {
		block->BreakBlock();
		if (block->BlockBroken()) {

			if (m_reactionTimer == 0.0f) {
				m_rocks->SetFalling(true);
				m_player->SetState(IDLE);
				m_player->Position() = m_grid->GetPlayerStartPos();
			}

			if (m_reactionTimer < m_reactionTime) {
				m_reactionTimer += deltaTime;

				if (m_reactionTimer > 1.0f && m_reactionTimer < 1.8f) {
					m_player->SetState(JUMP);
					m_player->Position() = m_grid->GetPlayerStartPos();
				}
				else {
					m_player->SetState(IDLE);
					m_player->Position() = m_grid->GetPlayerStartPos();
				}
			}

			if (m_reactionTimer >= m_reactionTime) {
				m_reactionTime = m_reactionTime;
				m_player->SetFlip(false);
			}
		}
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_ESCAPE) == BS_PRESSED)
	{
		Game::GetInstance().Quit();
	}
}

void StartCutscene::Draw(Renderer& renderer)
{
	m_grid->Draw(renderer);
	m_rocks->Draw(renderer);
	m_player->Draw(renderer);
}

void StartCutscene::DebugDraw()
{
}

bool StartCutscene::GetStatus()
{
	return false;
}
