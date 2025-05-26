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

    if (m_pWarehouseBackground) {
        delete m_pWarehouseBackground;
        m_pWarehouseBackground = nullptr;
    }
}

bool SceneMain::Initialise(Renderer& renderer)
{
    m_screenWidth = static_cast<float>(renderer.GetWidth());
    m_screenHeight = static_cast<float>(renderer.GetHeight());

    m_pWarehouseBackground = renderer.CreateSprite("../assets/background.png");


    float scaleX = static_cast<float>(renderer.GetWidth()) / m_pWarehouseBackground->GetWidth();
    float scaleY = static_cast<float>(renderer.GetHeight()) / m_pWarehouseBackground->GetHeight();
    float scale = std::max(scaleX, scaleY);  //ensuring background covers whole screen

    m_pWarehouseBackground->SetX(renderer.GetWidth() / 2);
    m_pWarehouseBackground->SetY(renderer.GetHeight() / 2);
    m_pWarehouseBackground->SetScale(scale);

    m_pPlayer = new Player();
    if (!m_pPlayer->Initialise(renderer))
    {
        delete m_pPlayer;
        m_pPlayer = nullptr;
        return false;
    }

    m_grid = new Grid();
    m_grid->Initialise(renderer);

    ui = std::make_unique<UI>(&renderer);

    return true;
}

void SceneMain::Process(float deltaTime, InputSystem& inputSystem)
{
    //XboxController* controller = inputSystem.GetController(0);

   //m_pTitleText->Process(deltaTime);
    if (m_pPlayer)
    {
        m_pPlayer->Process(deltaTime, inputSystem);
    }

    m_grid->Process(deltaTime, inputSystem);

    ui->Update(depth);
}

void SceneMain::Draw(Renderer& renderer)
{
    float playerX = static_cast<float>(m_pPlayer->GetPosition().x);
    float playerY = static_cast<float>(m_pPlayer->GetPosition().y);
    //playerX = 500.0f;
    std::cout << playerX << "   " << playerY << std::endl;
    renderer.SetCameraPosition(playerX, playerY);

    // Optional zoom logic:
    renderer.SetZoom(1.0f);

    if (m_pWarehouseBackground)
    {
        m_pWarehouseBackground->Draw(renderer);
    }

    m_grid->Draw(renderer);

    if (m_pPlayer) {
        m_pPlayer->Draw(renderer);
    }

    ui->Render();
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


