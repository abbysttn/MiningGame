#include "endcutscene.h"

#include "renderer.h"
#include "inputsystem.h"

#include "grid.h"
#include "block.h"
#include "game.h"
#include "cutsceneplayer.h"

#include "fallingrocks.h"

#include "logmanager.h"
#include "sprite.h"

EndCutscene::EndCutscene() : m_grid(nullptr), m_player(nullptr), m_rocks(nullptr), m_fade(nullptr), m_trappedMiner(nullptr) {}

EndCutscene::~EndCutscene()
{
	delete m_grid;
	m_grid = nullptr;

	delete m_player;
	m_player = nullptr;

	delete m_rocks;
	m_rocks = nullptr;

	delete m_fade;
	m_fade = nullptr;

	delete m_trappedMiner;
	m_trappedMiner = nullptr;

	m_tSoundSystem.Release();
}

bool EndCutscene::Initialise(Renderer& renderer)
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
	m_player->Position().x += m_grid->GetBlockSize().x * 15.0f;

	finishPos.x = m_grid->GetPlayerStartPos().x + m_grid->GetBlockSize().x * 3.0f;
	finishPos.y = m_player->Position().y;

	m_player->SetState(IDLE);
	m_player->SetFlip(true);

	m_trappedMiner = new CutscenePlayer();
	m_trappedMiner->Initialise(renderer);
	m_trappedMiner->SetScale(m_grid->GetTileSize() / 35.0f);
	m_trappedMiner->Position() = m_grid->GetPlayerStartPos();
	m_trappedMiner->SetColour(0.8f, 1.0f, 0.8f);

	m_trappedMiner->SetState(IDLE);
	m_trappedMiner->SetFlip(false);
	m_trappedMiner->SetRotation(45.0f);

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

void EndCutscene::Process(float deltaTime, InputSystem& inputSystem)
{
	m_grid->Process(deltaTime, inputSystem);

	m_rocks->Process(deltaTime, inputSystem);

	m_trappedMiner->Process(deltaTime);

	m_player->Process(deltaTime);

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

		if (m_player->Position().x >= finishPos.x) {
			m_player->Position().x -= 150.0f * deltaTime;
			m_rockTimer = 0.0f;
		}
		else if (!m_rocksFallen) {
			m_trappedMiner->SetRotation(0.0f);
			if (m_rockTimer >= 0.9f) m_trappedMiner->Position().x += 50.0f * deltaTime;
		}

		if (m_rockTimer < m_rockTime) {
			m_rockTimer += deltaTime;
			m_reactionTimer = 0.0f;
		}

		if (m_rockTimer >= m_rockTime) {
			m_rocks->SetFalling(true);
			m_rocksFallen = true;
		}

		if (m_reactionTimer < m_reactionTime) {
			m_reactionTimer += deltaTime;

			if (m_reactionTimer > 1.0f && m_reactionTimer < 1.8f) {
				m_trappedMiner->SetState(JUMP);
			}
			else {
				m_trappedMiner->SetState(IDLE);
			}

			if (m_reactionTimer > 1.2f && m_reactionTimer < 2.0f) {
				m_player->SetState(JUMP);
			}
			else {
				m_player->SetState(IDLE);
			}
		}

		if (m_reactionTimer >= m_reactionTime) {
			m_reactionTime = m_reactionTime;
			m_player->SetFlip(false);

			m_timer += deltaTime;
			alpha = m_timer - 3.0f;

			if (m_timer > 2.0f) {
				m_player->SetFlip(true);
			}

			if (m_timer >= m_time) {
				m_sceneDone = true;
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

void EndCutscene::Draw(Renderer& renderer)
{
	m_grid->Draw(renderer);
	m_rocks->Draw(renderer);
	m_player->Draw(renderer);
	m_trappedMiner->Draw(renderer);
	m_fade->Draw(renderer);
}

void EndCutscene::DebugDraw()
{
}

void EndCutscene::OnEnter()
{
	m_tSoundSystem.PlaySound("bgm");
}

void EndCutscene::OnExit()
{
	m_tSoundSystem.StopSound("bgm");
}

bool EndCutscene::IsFinished()
{
	return m_sceneDone;
}
