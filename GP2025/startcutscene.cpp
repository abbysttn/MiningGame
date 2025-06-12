#include "startcutscene.h"

#include "renderer.h"
#include "inputsystem.h"

#include "grid.h"
#include "block.h"
#include "game.h"
#include "cutsceneplayer.h"

#include "fallingrocks.h"

#include "logmanager.h"
#include "sprite.h"
#include "xboxcontroller.h"

StartCutscene::StartCutscene() : m_grid(nullptr), m_player(nullptr), m_rocks(nullptr), m_fade(nullptr) {}

StartCutscene::~StartCutscene()
{
	delete m_grid;
	m_grid = nullptr;

	delete m_player;
	m_player = nullptr;

	delete m_rocks;
	m_rocks = nullptr;

	delete m_fade;
	m_fade = nullptr;

	m_tSoundSystem.Release();
}

bool StartCutscene::Initialise(Renderer& renderer)
{
	if (!m_tSoundSystem.Initialise())
	{
		return false;
	}
	m_tSoundSystem.LoadSound("bgm", "../assets/sound/caveWind.mp3", true);
	m_tSoundSystem.LoadSound("fall", "../assets/sound/rockFall.mp3", false);
	m_tSoundSystem.LoadSound("hit", "../assets/sound/pickaxeHit.wav", false);

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

	m_fade = renderer.CreateSprite("../assets/fade.png");
	m_fade->SetX(renderer.GetWidth() / 2);
	m_fade->SetY(renderer.GetHeight() / 2);

	float scaleX = renderer.GetWidth() / (m_fade->GetWidth() * 0.25f);
	float scaleY = renderer.GetHeight() / (m_fade->GetHeight() * 0.25f);

	float scale = scaleX >= scaleY ? scaleX : scaleY;

	m_fade->SetScale(scale);
	m_fade->SetAlpha(alpha);

	return true;
}

void StartCutscene::Process(float deltaTime, InputSystem& inputSystem)
{
	m_player->SetColour(0.8f, 1.0f, 0.8f);

	m_grid->SetMiningStrength(5);
	m_grid->Process(deltaTime, inputSystem);

	m_rocks->Process(deltaTime, inputSystem);

	m_player->Process(deltaTime);

	// Skip Cutscene
	if (inputSystem.GetKeyState(SDL_SCANCODE_RETURN) == BS_PRESSED ||
		(inputSystem.GetNumberOfControllersAttached() > 0 &&
		inputSystem.GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_START) == BS_PRESSED))
	{
		m_tSoundSystem.PlaySound("click");
		m_sceneDone = true;
	}

	if (m_startTimer < m_startTime) {
		m_startTimer += deltaTime;

		if (!m_hitSound) {
			m_tSoundSystem.PlaySound("hit");
			m_hitSound = true;
		}

		if (m_startTimer >= 0.8f && !m_hitSound2) {
			m_tSoundSystem.PlaySound("hit");
			m_hitSound2 = true;
		}
	}

	if (m_startTimer >= m_startTime) {
		m_startTimer = m_startTime;
	}

	Vector2 gridCoords = { 4, 7 };

	Block* block = m_grid->GetBlockFromGrid(gridCoords);

	if (block != nullptr) {
		block->BreakBlock(false);
		if (block->BlockBroken()) {

			if (m_reactionTimer == 0.0f) {
				m_rocks->SetFalling(true);
				m_tSoundSystem.PlaySound("fall");
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

				m_timer += deltaTime;
				alpha = m_timer - 1.0f;

				if (m_timer >= m_time) {
					m_sceneDone = true;
				}
			}
		}
	}

	m_fade->SetAlpha(alpha);
	m_fade->Process(deltaTime);

	if (inputSystem.GetKeyState(SDL_SCANCODE_ESCAPE) == BS_PRESSED)
	{
		Game::GetInstance().Quit();
	}

	m_tSoundSystem.Update();
}

void StartCutscene::Draw(Renderer& renderer)
{
	m_grid->Draw(renderer);
	m_rocks->Draw(renderer);
	m_player->Draw(renderer);
	m_fade->Draw(renderer);
}

void StartCutscene::DebugDraw()
{
}

void StartCutscene::OnEnter(bool reset)
{
	m_tSoundSystem.PlaySound("bgm");
}

void StartCutscene::OnExit()
{
	m_tSoundSystem.StopSound("bgm");
}

bool StartCutscene::IsFinished()
{
	return m_sceneDone;
}
