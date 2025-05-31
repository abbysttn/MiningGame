#include "scenemain.h"
#include "renderer.h"

#include "texture.h"
#include "sprite.h"
#include "player.h"
#include "game.h"
#include "scene.h"
#include "xboxcontroller.h"
#include <iostream>
#include "ui.h"

#include "grid.h"

#include <fmod.hpp>
#include <fmod_errors.h>

#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"

SceneMain::SceneMain(FMOD::System* pFMODSystem) : m_grid(nullptr) {}

SceneMain::~SceneMain()
{
    delete m_pPlayer;
    m_pPlayer = nullptr;

    delete m_grid;
    m_grid = nullptr;

    if (m_pMineBackground) {
        delete m_pMineBackground;
        m_pMineBackground = nullptr;
    }
}

bool SceneMain::Initialise(Renderer& renderer)
{
    m_screenWidth = static_cast<float>(renderer.GetWidth());
    m_screenHeight = static_cast<float>(renderer.GetHeight());

    m_pMineBackground = renderer.CreateSprite("../assets/background.png");


    float scaleX = static_cast<float>(renderer.GetWidth()) / m_pMineBackground->GetWidth();
    float scaleY = static_cast<float>(renderer.GetHeight()) / m_pMineBackground->GetHeight();
    float scale = std::max(scaleX, scaleY);  //ensuring background covers whole screen

    m_pMineBackground->SetX(renderer.GetWidth() / 2);
    float scaledHeight = m_pMineBackground->GetHeight() * scale;
    m_pMineBackground->SetY(static_cast<int>(scaledHeight / 2.0f));
    m_pMineBackground->SetScale(scale);

    m_pPlayer = new Player();
    if (!m_pPlayer->Initialise(renderer))
    {
        delete m_pPlayer;
        m_pPlayer = nullptr;
        return false;
    }

    m_grid = new Grid();
    m_grid->Initialise(renderer);

    m_tileSize = m_grid->GetTileSize();

    ui = std::make_unique<UI>(&renderer);
    m_screenX = renderer.GetWidth() / 2;
    m_playerY = static_cast<float>(m_pPlayer->GetPosition().y);

    renderer.SetCameraPosition(static_cast<float>(m_screenX), m_pMineBackground->GetHeight() * 0.1f);

    return true;
}

void SceneMain::Process(float deltaTime, InputSystem& inputSystem)
{
    if (m_pPlayer)
    {
        m_pPlayer->Process(deltaTime, inputSystem);
        m_pPlayer->SetDepth(static_cast<int>((m_pPlayer->GetPosition().y / m_tileSize) - m_aboveGroundOffset));
    }

	if (inputSystem.GetKeyState(SDL_SCANCODE_K))
	{
		m_pPlayer->SetHealth(m_pPlayer->GetHealth() - 0.5f);
	}
    if (inputSystem.GetKeyState(SDL_SCANCODE_L))
    {
        m_pPlayer->SetStamina(m_pPlayer->GetStamina() - 0.5f);
    }

    m_grid->Process(deltaTime, inputSystem);

    ui->Update(m_pPlayer, m_pRenderer);
}

void SceneMain::Draw(Renderer& renderer){

    MoveCamera(renderer);

    // Optional zoom logic:
    renderer.SetZoom(1.0f);

    if (m_pMineBackground)
    {
        m_pMineBackground->Draw(renderer);
    }

    m_grid->Draw(renderer);

    if (m_pPlayer) {
        m_pPlayer->Draw(renderer);
    }

    ui->Render(); // Draw Last
}


void SceneMain::DebugDraw()
{
    if (m_pPlayer)
    {
        ImGui::NewLine();
        ImGui::Text("Press Spacebar to hide/show");
        ImGui::Text("Debugging Tools:");
    }
}

int SceneMain::GetBackgroundHeight() {
    return m_pMineBackground->GetHeight();
}
void SceneMain::MoveCamera(Renderer& renderer) {


    m_playerY = static_cast<float>(m_pPlayer->GetPosition().y);
    float scrollStart = static_cast<float>(m_pMineBackground->GetHeight())*0.1f;
    float scrollStop = static_cast<float>(m_pMineBackground->GetHeight())*0.8315f; //change


    if (m_playerY >= scrollStart && m_playerY <= scrollStop) {
        renderer.SetCameraPosition(static_cast<float>(m_screenX), m_playerY);

    }


}


