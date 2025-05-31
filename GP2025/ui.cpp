#include "ui.h"
#include "renderer.h"
#include "texture.h"
#include "logmanager.h"
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

	// Set Initial Depth
	m_depthText = "Depth: 0m";
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
	std::string newText = "Depth: " + std::to_string(player->GetDepth()) + "m";
	if (newText != m_depthText) {
		m_depthText = newText;
		m_depthTexture.LoadTextTexture(m_pRenderer->GetSDLRenderer(), m_depthText.c_str(), *m_font, white);
		m_depthSprite.reset(m_pRenderer->CreateTextSprite(&m_depthTexture));
		m_depthSprite->SetX(static_cast<int>(m_depthTextPos.x));
		m_depthSprite->SetY(static_cast<int>(m_depthTextPos.y));
	}

	// HEALTH AND STAMINA BARS
	healthPercent = static_cast<float>(player->GetHealth()) / 100.0f; 
	staminaPercent = static_cast<float>(player->GetStamina()) / 100.0f; 
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

}

void UI::LoadDefaultTexture() {
	// Create initial texture and sprite
	m_depthTexture.LoadTextTexture(m_pRenderer->GetSDLRenderer(), m_depthText.c_str(), *m_font, white);
	m_depthSprite.reset(m_pRenderer->CreateTextSprite(&m_depthTexture));
	m_depthSprite->SetX(static_cast<int>(m_depthTextPos.x));
	m_depthSprite->SetY(static_cast<int>(m_depthTextPos.y));
}