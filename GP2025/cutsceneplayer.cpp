#include "cutsceneplayer.h"

#include "renderer.h"
#include "animatedsprite.h"

CutscenePlayer::CutscenePlayer() {}

CutscenePlayer::~CutscenePlayer()
{
	delete m_pMineSprite;
	m_pMineSprite = nullptr;
	delete m_pIdleSprite;
	m_pIdleSprite = nullptr;
	delete m_pJumpSprite;
	m_pJumpSprite = nullptr;
}

bool CutscenePlayer::Initialise(Renderer& renderer)
{
	m_pIdleSprite = renderer.CreateAnimatedSprite("../assets/Idle.png");
	m_pMineSprite = renderer.CreateAnimatedSprite("../assets/Swing.png");
	m_pJumpSprite = renderer.CreateAnimatedSprite("../assets/Jump.png");

	m_pIdleSprite->SetupFrames(32, 32);
	m_pIdleSprite->SetLooping(true);
	m_pIdleSprite->SetFrameDuration(0.1f);
	m_pIdleSprite->Animate();
	m_pIdleSprite->SetScale(m_scale);

	m_pMineSprite->SetupFrames(32, 32);
	m_pMineSprite->SetLooping(true);
	m_pMineSprite->SetFrameDuration(0.1f);
	m_pMineSprite->Animate();
	m_pMineSprite->SetScale(m_scale);

	m_pJumpSprite->SetupFrames(32, 32);
	m_pJumpSprite->SetLooping(true);
	m_pJumpSprite->SetFrameDuration(0.1f);
	m_pJumpSprite->Animate();
	m_pJumpSprite->SetScale(m_scale);

	// Default
	m_pAnimSprite = m_pIdleSprite;
	m_animationState = IDLE;

	if (m_pAnimSprite == nullptr)
	{
		return false;
	}

	return true;
}

void CutscenePlayer::Process(float deltaTime)
{
	m_pAnimSprite->SetScale(m_scale);

	m_pAnimSprite->SetX(static_cast<int>(m_position.x));
	m_pAnimSprite->SetY(static_cast<int>(m_position.y));
	m_pAnimSprite->Process(deltaTime);
}

void CutscenePlayer::Draw(Renderer& renderer)
{
	m_pAnimSprite->Draw(renderer, m_flip);
}

Vector2& CutscenePlayer::Position()
{
	return m_position;
}

void CutscenePlayer::SetScale(float scale)
{
	m_scale = scale;
}

void CutscenePlayer::SetFlip(bool flip)
{
	m_flip = flip;
}

void CutscenePlayer::SetState(PlayerState state)
{
	m_animationState = state;

	switch (state) {
	case MINE:
		m_pAnimSprite = m_pMineSprite;
		m_pAnimSprite->SetScale(m_scale);
		m_pAnimSprite->SetX(static_cast<int>(m_position.x));
		m_pAnimSprite->SetY(static_cast<int>(m_position.y));
		m_pMineSprite->Animate();
		break;

	case IDLE:
		m_pAnimSprite = m_pIdleSprite;
		m_pAnimSprite->SetScale(m_scale);
		m_pAnimSprite->SetX(static_cast<int>(m_position.x));
		m_pAnimSprite->SetY(static_cast<int>(m_position.y));
		m_pIdleSprite->Animate();
		break;
		
	case JUMP:
		m_pAnimSprite = m_pJumpSprite;
		m_pAnimSprite->SetScale(m_scale);
		m_pAnimSprite->SetX(static_cast<int>(m_position.x));
		m_pAnimSprite->SetY(static_cast<int>(m_position.y));
		m_pJumpSprite->Animate();
		break;
	}
}

void CutscenePlayer::SetCutscene(CutScene scene)
{
	m_currentScene = scene;
}
