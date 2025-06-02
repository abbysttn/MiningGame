#include "ui.h"
#include "renderer.h"
#include "texture.h"
#include "logmanager.h"
#include "texturemanager.h"
#include "sprite.h"
#include "texture.h"
#include "font.h"
#include "SDL.h"
#include "player.h"
#include <iostream>

UI::UI(Renderer* renderer)
	: m_pRenderer(renderer)
	, m_font(std::make_unique<Font>("../assets/fonts/joystix.otf", 24))
{
	m_screenWidth = static_cast<float>(m_pRenderer->GetWidth());
	m_screenHeight = static_cast<float>(m_pRenderer->GetHeight());

	m_depthTextPos.x = m_screenWidth - 150.0f; 
	m_depthTextPos.y = 50.0f;

	// Set Initial Texts
	m_depthText = "Depth: 0m";
	m_dirtCountText = "0";
	m_stoneCountText = "0";
	m_gemCountText = "0";

	LoadDefaultTexture();

	// Set health and stamina bar position
	barPositionHealth = Vector2(120.0f, m_screenHeight - 70.0f); 
	barPositionStamina = Vector2(120.0f, m_screenHeight - 35.0f);
}

UI::~UI()
{
	
}

void UI::Update(Player* player, Renderer* renderer)
{
	// DEPTH TRACKER
	std::string newDepthText = "Depth: " + std::to_string(player->GetDepth()) + "m";
	if (newDepthText != m_depthText) {
		m_depthText = newDepthText;
		m_depthTexture.LoadTextTexture(m_pRenderer->GetSDLRenderer(), m_depthText.c_str(), *m_font, white);
		m_depthSprite.reset(m_pRenderer->CreateTextSprite(&m_depthTexture));
		m_depthSprite->SetX(static_cast<int>(m_depthTextPos.x));
		m_depthSprite->SetY(static_cast<int>(m_depthTextPos.y));
	}

	// HEALTH AND STAMINA BARS
	healthPercent = static_cast<float>(player->GetHealth()) / 100.0f; 
	staminaPercent = static_cast<float>(player->GetStamina()) / 100.0f; 

	// RESOURCE COUNTERS
	std::string newDirtText = std::to_string(player->GetDirt());
	if (newDirtText != m_dirtCountText) {
		m_dirtCountText = newDirtText;
		m_dirtCountTexture.LoadTextTexture(m_pRenderer->GetSDLRenderer(), m_dirtCountText.c_str(), *m_font, white);
		m_dirtCountSprite.reset(m_pRenderer->CreateTextSprite(&m_dirtCountTexture));
		m_dirtCountSprite->SetX(110);
		m_dirtCountSprite->SetY(45);
	}

	std::string newStoneText = std::to_string(player->GetStone());
	if (newStoneText != m_stoneCountText) {
		m_stoneCountText = newStoneText;
		m_stoneCountTexture.LoadTextTexture(m_pRenderer->GetSDLRenderer(), m_stoneCountText.c_str(), *m_font, white);
		m_stoneTextSprite.reset(m_pRenderer->CreateTextSprite(&m_stoneCountTexture));
		m_stoneTextSprite->SetX(110);
		m_stoneTextSprite->SetY(95);
	}

	std::string newGemText = std::to_string(player->GetGem());
	if (newGemText != m_gemCountText) {
		m_gemCountText = newGemText;
		m_gemCountTexture.LoadTextTexture(m_pRenderer->GetSDLRenderer(), m_gemCountText.c_str(), *m_font, white);
		m_gemTextSprite.reset(m_pRenderer->CreateTextSprite(&m_gemCountTexture));
		m_gemTextSprite->SetX(110);
		m_gemTextSprite->SetY(145);
	}
}

void UI::Render()
{
	if (m_depthSprite)
	{
		m_pRenderer->DrawSpriteScreenSpace(*m_depthSprite);
	}

	// Background bars
	m_pRenderer->DrawRectScreenSpace(barPositionHealth, Vector2(barWidth, barHeight), 0.2f, 0.0f, 0.0f, 1.0f);
	m_pRenderer->DrawRectScreenSpace(barPositionStamina, Vector2(barWidth, barHeight), 0.0f, 0.2f, 0.0f, 1.0f); // dark green

	// Foreground bars
	m_pRenderer->DrawRectScreenSpace(barPositionHealth, Vector2(barWidth * healthPercent, barHeight), 1.0f, 0.0f, 0.0f, 1.0f); // red
	m_pRenderer->DrawRectScreenSpace(barPositionStamina, Vector2(barWidth * staminaPercent, barHeight), 0.0f, 1.0f, 0.0f, 1.0f); // green

	// Resource Counters
	if (m_dirtIcon) {
		m_pRenderer->DrawSpriteScreenSpace(*m_dirtIcon);
	}
	if (m_dirtCountSprite) {
		m_pRenderer->DrawSpriteScreenSpace(*m_dirtCountSprite);
	}
	if (m_stoneIcon) {
		m_pRenderer->DrawSpriteScreenSpace(*m_stoneIcon);
	}
	if (m_stoneTextSprite) {
		m_pRenderer->DrawSpriteScreenSpace(*m_stoneTextSprite);
	}
	if (m_gemIcon) {
		m_pRenderer->DrawSpriteScreenSpace(*m_gemIcon);
	}
	if (m_gemTextSprite) {
		m_pRenderer->DrawSpriteScreenSpace(*m_gemTextSprite);
	}
}

// Create initial textures and sprites
void UI::LoadDefaultTexture() {
	// Depth
	m_depthTexture.LoadTextTexture(m_pRenderer->GetSDLRenderer(), m_depthText.c_str(), *m_font, white);
	m_depthSprite.reset(m_pRenderer->CreateTextSprite(&m_depthTexture));
	m_depthSprite->SetX(static_cast<int>(m_depthTextPos.x));
	m_depthSprite->SetY(static_cast<int>(m_depthTextPos.y));

	// Resource Counter Icons
	m_dirtIcon.reset(m_pRenderer->CreateSprite("../assets/dirt_icon.png"));
	m_dirtIcon->SetScale(0.35f);
	m_dirtIcon->SetX(40);
	m_dirtIcon->SetY(40);

	m_stoneIcon.reset(m_pRenderer->CreateSprite("../assets/stone_icon.png"));
	m_stoneIcon->SetScale(0.5f);
	m_stoneIcon->SetX(40);
	m_stoneIcon->SetY(90);

	m_gemIcon.reset(m_pRenderer->CreateSprite("../assets/gem_icon.png"));
	m_gemIcon->SetScale(0.35f);
	m_gemIcon->SetX(40);
	m_gemIcon->SetY(150);

	// Resource Counter Text
	m_dirtCountTexture.LoadTextTexture(m_pRenderer->GetSDLRenderer(), m_dirtCountText.c_str(), *m_font, white);
	m_dirtCountSprite.reset(m_pRenderer->CreateTextSprite(&m_dirtCountTexture));
	m_dirtCountSprite->SetX(90);
	m_dirtCountSprite->SetY(45);

	m_stoneCountTexture.LoadTextTexture(m_pRenderer->GetSDLRenderer(), m_stoneCountText.c_str(), *m_font, white);
	m_stoneTextSprite.reset(m_pRenderer->CreateTextSprite(&m_stoneCountTexture));
	m_stoneTextSprite->SetX(90);
	m_stoneTextSprite->SetY(95);

	m_gemCountTexture.LoadTextTexture(m_pRenderer->GetSDLRenderer(), m_gemCountText.c_str(), *m_font, white);
	m_gemTextSprite.reset(m_pRenderer->CreateTextSprite(&m_gemCountTexture));
	m_gemTextSprite->SetX(90);
	m_gemTextSprite->SetY(145);
}