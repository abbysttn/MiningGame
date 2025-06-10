#include "startcutscene.h"

#include "renderer.h"
#include "inputsystem.h"

#include "grid.h"
#include "game.h"
#include "player.h"

StartCutscene::StartCutscene() : m_grid(nullptr), m_player(nullptr) {}

StartCutscene::~StartCutscene()
{
	delete m_grid;
	m_grid = nullptr;

	delete m_player;
	m_player = nullptr;
}

bool StartCutscene::Initialise(Renderer& renderer)
{
	m_grid = new Grid();
	m_grid->SetBackgroundHeight(renderer.GetHeight() - (m_grid->GetTileSize() * 20.0f));
	m_grid->SetGridType(false);
	m_grid->Initialise(renderer);

	m_player = new Player();
	//m_player->Initialise(renderer);
	//m_player->GetPosition() = m_grid->GetPlayerStartPos();

	return true;
}

void StartCutscene::Process(float deltaTime, InputSystem& inputSystem)
{
	m_grid->Process(deltaTime, inputSystem);
	//m_player->Process(deltaTime, inputSystem);

	if (inputSystem.GetKeyState(SDL_SCANCODE_ESCAPE) == BS_PRESSED)
	{
		Game::GetInstance().Quit();
	}
}

void StartCutscene::Draw(Renderer& renderer)
{
	m_grid->Draw(renderer);
	//m_player->Draw(renderer);
}

void StartCutscene::DebugDraw()
{
}

bool StartCutscene::GetStatus()
{
	return false;
}
