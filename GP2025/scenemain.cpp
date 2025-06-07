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
#include "logmanager.h"

#include "gridstate.h"
#include "collisionhelper.h"

#include <fmod.hpp>
#include <fmod_errors.h>

#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"

SceneMain::SceneMain() 
    : m_tileSize(0.0f)
    , m_screenWidth(0.0f)
    , m_screenHeight(0.0f)
    , m_pMineBackground(nullptr)
    , m_pPlayer(nullptr)
    , m_pCoinSprite(nullptr)
    , m_pDirtSprite(nullptr)
    , m_pBreakBlockSprite(nullptr)
    , ui(nullptr), m_testSpider(nullptr)
{}

SceneMain::~SceneMain()
{
    delete m_pPlayer;
    m_pPlayer = nullptr;

    delete m_testSpider;
    m_testSpider = nullptr;

    GridState::GetInstance().ResetGrid();



    //ADD: delete sprites function


    if (m_pMineBackground) {
        delete m_pMineBackground;
        m_pMineBackground = nullptr;
    }

    if (m_pCoinSprite) {
        delete m_pCoinSprite;
        m_pCoinSprite = nullptr;
    }


    if (m_pDirtSprite) {
        delete m_pDirtSprite;
        m_pDirtSprite = nullptr;

    }

    if (m_pBreakBlockSprite) {
        delete m_pBreakBlockSprite;
        m_pBreakBlockSprite = nullptr;
    }

    if (m_pWaterDropSprite) {
        delete m_pWaterDropSprite;
        m_pWaterDropSprite = nullptr;
    }


    if (m_pVignetteSprite) {
        delete m_pVignetteSprite;
        m_pVignetteSprite = nullptr;
    }

    if (m_pDirtPickupSprite) {
        delete m_pDirtPickupSprite;
        m_pDirtPickupSprite = nullptr;
    }

    if (m_pStonePickupSprite) {
        delete m_pStonePickupSprite;
        m_pStonePickupSprite = nullptr;
    }
    
    m_soundSystem.Release();

}

void SceneMain::OnEnter() {
    m_paused = false;
};

void SceneMain::OnExit()
{
}
void SceneMain::CheckCollision(Player* player, SpiderState* spider)
{
    Box playerBox(m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y, m_pPlayer->GetPlayerHeight(), m_pPlayer->GetPlayerHeight());

    auto potentialCollisions = m_collisionTree->queryRange(playerBox);

    for (auto* obj : potentialCollisions) {
        if (SpiderState* spider = dynamic_cast<SpiderState*>(obj)) {
            Vector2 pushDirection(spider->GetPosition().x - player->GetPosition().x,
                spider->GetPosition().y - player->GetPosition().y);

            if (spider->GetState() != SpiderStates::DIE || spider->GetState() == SpiderStates::HURT) {
                if (m_pPlayer->GetCurrentState() == PlayerAnimationState::MINE) {
                    spider->SetState(SpiderStates::HURT);
                    spider->ApplyPushback(pushDirection);
                }
                else {
                    spider->SetState(SpiderStates::HURT);
                    spider->ApplyPushback(pushDirection);
                    m_pPlayer->SetHealth(m_pPlayer->GetHealth() - 1.0f);
                }
            }
        }
    }
}

bool SceneMain::Initialise(Renderer& renderer)
{
    LogManager::GetInstance().Log("SceneMain is Initialising!");

    m_pVisionLevel = 1;
    m_visionLevels = { 1.0f, 1.2f, 1.3f, 1.5f, 2.0f };

    m_dirtParticleCooldown = 0.5f;
    if (!m_soundSystem.Initialise()) {
        std::cerr << "Failed to initialise FMOD system!" << std::endl;
        return false;
    }
    m_soundSystem.LoadSound("waterdrop", "../assets/sound/waterDrop.wav");

    m_pRenderer = &renderer;
    m_screenWidth = static_cast<float>(renderer.GetWidth());
    m_screenHeight = static_cast<float>(renderer.GetHeight());

    m_pMineBackground = renderer.CreateSprite("../assets/background.png");
   
    m_pVignetteSprite = renderer.CreateSprite("../assets/vignette.png");
    m_pVignetteSprite->SetScale(m_pVisionLevel);


    float scaleX = static_cast<float>(renderer.GetWidth()) / m_pMineBackground->GetWidth();
    float scaleY = static_cast<float>(renderer.GetHeight()) / m_pMineBackground->GetHeight();
    float scale = std::max(scaleX, scaleY);  //ensuring background covers whole screen

    m_pMineBackground->SetX(renderer.GetWidth() / 2);
    float scaledHeight = m_pMineBackground->GetHeight() * scale;
    m_pMineBackground->SetY(static_cast<int>(scaledHeight / 2.0f));
    m_pMineBackground->SetScale(scale);

    GridState::GetInstance().CreateGrid(renderer, scaledHeight);

    m_pPlayer = new Player();
    if (!m_pPlayer->Initialise(renderer))
    {
        delete m_pPlayer;
        m_pPlayer = nullptr;
        return false;
    }

    m_tileSize = GridState::GetInstance().GetTileSize();

    ui = std::make_unique<UI>(&renderer);
    m_screenX = renderer.GetWidth();
    m_playerY = static_cast<float>(m_pPlayer->GetPosition().y);


    //init particles
    m_pCoinSprite = renderer.CreateSprite("../assets/ball.png");
    m_pCoinSprite->SetScale(0.05f);

    m_pDirtSprite = renderer.CreateSprite("../assets/dirtparticle.png");
    m_pBreakBlockSprite = renderer.CreateSprite("../assets/dirtparticle.png");
    m_pWaterDropSprite = renderer.CreateSprite("../assets/particle.png");

    m_pDirtPickupSprite = renderer.CreateSprite("../assets/dirt_icon.png");
    m_pStonePickupSprite = renderer.CreateSprite("../assets/stone_icon.png");
    m_pGemPickupSprite = renderer.CreateSprite("../assets/gem_icon.png");

    renderer.SetCameraPosition(static_cast<float>(m_screenX/2), m_pMineBackground->GetHeight() * 0.1f);

    m_testSpider = new GameObjectPool(SpiderState(), 10);

    for (size_t i = 0; i < m_testSpider->totalCount(); i++) {
        if (GameObject* obj = m_testSpider->getObjectAtIndex(i)) {
            SpiderState* spider = dynamic_cast<SpiderState*>(obj);
            spider->InitialiseSpiders(renderer, m_screenX, (int)scaledHeight);
            spider->SetActive(false);
        }
    }

    m_collisionTree = make_unique<QuadTree>(Box(0.0f, 0.0f, (float)renderer.GetWidth(), scaledHeight));

    m_bIsInitialised = true;
    LogManager::GetInstance().Log("SceneMain Initialized complete");
    return true;
}

void SceneMain::Process(float deltaTime, InputSystem& inputSystem)
{
    m_collisionTree->clear();
    
    //quit to menu
    ButtonState escapeState = inputSystem.GetKeyState(SDL_SCANCODE_ESCAPE);
    if (escapeState == BS_PRESSED)
    {
        m_paused = true;
        std::cout << "Escape pressed" << std::endl;
        Game::GetInstance().SetCurrentScene(0);
    }


    if (!m_paused) {
        m_timer += deltaTime;

        if (m_pPlayer)
        {
            m_pPlayer->Process(deltaTime, inputSystem);
            m_pPlayer->SetDepth(static_cast<int>((m_pPlayer->GetPosition().y / m_tileSize) - m_aboveGroundOffset));
        }


        TestingFeatures(inputSystem);

        float pX = m_pPlayer->GetPosition().x;
        float pY = m_pPlayer->GetPosition().y/* - m_pPlayer->GetPlayerHeight() / 4.0f*/;
        Vector2 pPos = Vector2(pX, pY);


        int type = GridState::GetInstance().GetLastBlockType();

        Sprite* pickupSprite = m_pDirtPickupSprite;


        switch (type) {
        case 0: 
            pickupSprite = m_pDirtPickupSprite;

            break;

        case 1: 
            pickupSprite = m_pStonePickupSprite;

            break;
        case 2:
            pickupSprite = m_pGemPickupSprite;

            break;
        default:

            break;
        }


        m_dirtParticleCooldown -= deltaTime;
        if (GridState::GetInstance().CheckBlockDig() && m_dirtParticleCooldown <= 0.0f) {


            ParticleSystem ps;
            ps.Initialise(m_pDirtSprite, m_pPlayer, 1, ParticleType::DigDirt);
            ps.ActivateAt(pPos);
            m_particleSystems.push_back(std::move(ps));
            m_dirtParticleCooldown = 0.35f;
        }

        if (GridState::GetInstance().CheckBlockBreak()) {
            ParticleSystem ps;
            ps.Initialise(m_pBreakBlockSprite, m_pPlayer, 35, ParticleType::BlockBreak);
            ps.ActivateAt(pPos);
            m_particleSystems.push_back(std::move(ps));

            ParticleSystem ps2;
            ps2.Initialise(pickupSprite, m_pPlayer, 5);
            ps2.ActivateAt(m_pPlayer->GetPosition());
            m_particleSystems.push_back(std::move(ps2));
        }





        SpawnWaterDrops();

        if (GridState::GetInstance().IsBlockBroken() && GridState::GetInstance().SpiderSpawn()) {
            if (m_testSpider->usedCount() != m_testSpider->totalCount()) {
                if (GameObject* obj = m_testSpider->getObject()) {
                    if (obj && dynamic_cast<SpiderState*>(obj)) {
                        SpiderState* spider = dynamic_cast<SpiderState*>(obj);

                        spider->SetActive(true);
                        spider->SetPosition(GridState::GetInstance().GetBrokenBlockPos());
                    }
                }
            }
        }

        for (size_t i = 0; i < m_testSpider->totalCount(); i++) {
            if (GameObject* obj = m_testSpider->getObjectAtIndex(i)) {
                if (obj && dynamic_cast<SpiderState*>(obj)) {
                    SpiderState* spider = dynamic_cast<SpiderState*>(obj);
                    if (spider->IsActive()) {

                        Box spiderBox(
                            spider->GetPosition().x,
                            spider->GetPosition().y,
                            (float)spider->GetSpriteWidth(),
                            (float)spider->GetSpriteHeight()
                        );

                        m_collisionTree->insert(spider, spiderBox);

                        CheckCollision(m_pPlayer, spider);

                        spider->Update(deltaTime, m_pPlayer->GetPosition());
                    }
                }
            }
        }

        GridState::GetInstance().ProcessGrid(deltaTime, inputSystem);


        m_pVignetteSprite->SetX(static_cast<int>(m_pPlayer->GetPosition().x));
        m_pVignetteSprite->SetY(static_cast<int>(m_pPlayer->GetPosition().y));

        ui->Update(m_pPlayer, m_pRenderer);


        ProcessParticles(deltaTime);


        m_soundSystem.Update();
    }



}

void SceneMain::Draw(Renderer& renderer){

    MoveCamera(renderer);

    // Optional zoom logic:
    renderer.SetZoom(1.0f);

    if (m_pMineBackground)
    {
        m_pMineBackground->Draw(renderer);
    }

    GridState::GetInstance().DrawGrid(renderer);

    if (m_pPlayer) {
        m_pPlayer->Draw(renderer);
    }

    for (size_t i = 0; i < m_testSpider->totalCount(); i++) {
        if (GameObject* obj = m_testSpider->getObjectAtIndex(i)) {
            if (obj && obj->IsActive()) {
                SpiderState* spider = dynamic_cast<SpiderState*>(obj);
                if (spider->IsActive()) {
                    spider->Draw(renderer);
                }
            }
        }
    }

    //draw active particles
    for (auto& ps : m_particleSystems) {
        ps.Draw(renderer);
    }

    if (m_pVignetteSprite) {
        m_pVignetteSprite->Draw(renderer);
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
        ImGui::Text("%.1f FPS | Frame time: %.3f ms", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

    }
}

int SceneMain::GetBackgroundHeight() 
{
    return m_pMineBackground->GetHeight();
}

void SceneMain::MoveCamera(Renderer& renderer) {
    m_playerY = static_cast<float>(m_pPlayer->GetPosition().y);

    float bgHeight = static_cast<float>(m_pMineBackground->GetHeight());
    float screenHeight = static_cast<float>(renderer.GetHeight());

    float scrollStart = screenHeight / 2.0f; // Player must be this far from top before scrolling starts
    float scrollStop = bgHeight - screenHeight / 2.0f; // Player must be this far from bottom before scrolling stops

    if (m_playerY >= scrollStart && m_playerY <= scrollStop) {
        renderer.SetCameraPosition(static_cast<float>(m_screenX/2), m_playerY);
    }
    else if (m_playerY < scrollStart) {
        renderer.SetCameraPosition(static_cast<float>(m_screenX/2), scrollStart);
    }
    else if (m_playerY > scrollStop) {
        renderer.SetCameraPosition(static_cast<float>(m_screenX/2), scrollStop);
    }
}


void SceneMain::ProcessParticles(float time) {
//update active particles and clear old dead particles
    for (auto it = m_particleSystems.begin(); it != m_particleSystems.end(); ) {

        it->Update(time);

        for (Particle& p : it->GetParticles()) {
            if (p.GetType() == ParticleType::WaterDrop) {
                //play sound if velocity is ~zero and hasn't played yet
                std::cout << p.GetVelocity().y << std::endl;
                if (!p.CheckPlayedSound() && p.GetVelocity().y < 0.1f) {
                    m_soundSystem.PlaySound("waterdrop");
                    p.SetSoundPlayed();
                }
            }
        }
        //erase if dead
        if (it->IsFinished()) {
            it = m_particleSystems.erase(it);
        }
        else {
            ++it;
        }
    }
}

void SceneMain::SpawnWaterDrops() {
    //spawn water drops randomly
    if (m_timer >= m_waterDropInterval)
    {
        float randX = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * static_cast<float>(m_screenX);
        Vector2 dropSpawn = Vector2(randX, 0.0f);
        ParticleSystem ps;
        ps.Initialise(m_pWaterDropSprite, m_pPlayer, 1, ParticleType::WaterDrop);
        ps.ActivateAt(dropSpawn);
        m_particleSystems.push_back(std::move(ps));
        m_timer = 0.0f;
        m_waterDropInterval = 1.0f + static_cast<float>(rand()) / RAND_MAX * 3.0f;
    }
}

void SceneMain::TestingFeatures(InputSystem& inputSystem) {

    //test health and stamina

    // DEBUG STUFF
    if (inputSystem.GetKeyState(SDL_SCANCODE_K))
    {
        m_pPlayer->SetHealth(m_pPlayer->GetHealth() - 0.5f);
    }
    if (inputSystem.GetKeyState(SDL_SCANCODE_L))
    {
        m_pPlayer->SetStamina(m_pPlayer->GetStamina() - 0.5f);
    }
    if (inputSystem.GetKeyState(SDL_SCANCODE_J))
    {
        m_pPlayer->SetDirt(m_pPlayer->GetDirt() + 1);
    }
    if (inputSystem.GetKeyState(SDL_SCANCODE_H))
    {
        m_pPlayer->SetStone(m_pPlayer->GetStone() + 1);
    }
    if (inputSystem.GetKeyState(SDL_SCANCODE_G))
    {
        m_pPlayer->SetGem(m_pPlayer->GetGem() + 1);
    }

    //test particles

    if (inputSystem.GetKeyState(SDL_SCANCODE_I) == BS_PRESSED)
    {
        ParticleSystem ps;
        ps.Initialise(m_pCoinSprite, m_pPlayer, 5);
        ps.ActivateAt(m_pPlayer->GetPosition());
        m_particleSystems.push_back(std::move(ps));
    }

    if (inputSystem.GetKeyState(SDL_SCANCODE_O) == BS_PRESSED)
    {
        ParticleSystem ps;
        ps.Initialise(m_pDirtSprite, m_pPlayer, 5, ParticleType::DigDirt);
        ps.ActivateAt(m_pPlayer->GetPosition());
        m_particleSystems.push_back(std::move(ps));
    }
    if (inputSystem.GetKeyState(SDL_SCANCODE_U) == BS_PRESSED)
    {
        ParticleSystem ps;
        ps.Initialise(m_pBreakBlockSprite, m_pPlayer, 50, ParticleType::BlockBreak);
        ps.ActivateAt(m_pPlayer->GetPosition());
        m_particleSystems.push_back(std::move(ps));
    }
    if ((inputSystem.GetKeyState(SDL_SCANCODE_Y) == BS_PRESSED)&& m_pVisionLevel < m_visionLevels.size())
    {
        m_pVisionLevel++;
        SetVisionLevel((int)m_pVisionLevel);
    }
    if ((inputSystem.GetKeyState(SDL_SCANCODE_T) == BS_PRESSED) && m_pVisionLevel > 1)
    {
        m_pVisionLevel--;
        SetVisionLevel((int)m_pVisionLevel);
    }
}

void SceneMain::SetVisionLevel(int level) {
    m_pVisionLevel = (float)level;
    float newScale = m_visionLevels[level-1];
    m_pVignetteSprite->SetScale(newScale);
}
