#include "ui.h"
#include "renderer.h"
#include "texture.h"
#include "logmanager.h"
#include "sprite.h"
#include "texture.h"
#include "font.h"
#include "SDL.h"

UI::UI(Renderer* renderer)
	: m_SDLRenderer(renderer)
	, m_font(std::make_unique<Font>("../assets/fonts/joystix.otf", 24))
	, m_depthTextPos(10.0f, 10.0f)
{
	// Set Initial Depth
	m_depthText = "Depth: 0m";
	LoadDefaultTexture();
}

UI::~UI()
{
	
}

void UI::Update(int currentDepth)
{
	std::string newText = "Depth: " + std::to_string(currentDepth) + "m";
	if (newText != m_depthText) {
		m_depthText = newText;
		m_depthTexture.LoadTextTexture(m_SDLRenderer->GetSDLRenderer(), m_depthText.c_str(), *m_font, white);
		m_depthSprite.reset(m_SDLRenderer->CreateTextSprite(&m_depthTexture));
		m_depthSprite->SetX(m_depthTextPos.x);
		m_depthSprite->SetY(m_depthTextPos.y);
	}
}

void UI::Render()
{
	if (m_depthSprite) {
		m_depthSprite->Draw(*m_SDLRenderer);
	}
}

void UI::LoadDefaultTexture() {
	// Create initial texture and sprite
	m_depthTexture.LoadTextTexture(m_SDLRenderer->GetSDLRenderer(), m_depthText.c_str(), *m_font, white);
	m_depthSprite.reset(m_SDLRenderer->CreateTextSprite(&m_depthTexture));
	m_depthSprite->SetX(static_cast<int>(m_depthTextPos.x));
	m_depthSprite->SetY(static_cast<int>(m_depthTextPos.y));
}