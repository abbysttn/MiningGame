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
#include "resourcetype.h"

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
    , m_pActiveUpgradeStation(nullptr)
    , m_showUpgradePrompt(false)
    , m_isUpgradeMenuUIVisible(false)
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

    if (m_pDarkVignetteSprite) {
        delete m_pDarkVignetteSprite;
        m_pDarkVignetteSprite = nullptr;
    }

    if (m_pDirtPickupSprite) {
        delete m_pDirtPickupSprite;
        m_pDirtPickupSprite = nullptr;
    }

    if (m_pStonePickupSprite) {
        delete m_pStonePickupSprite;
        m_pStonePickupSprite = nullptr;
    }

    if (m_pGemPickupSprite) {
        delete m_pGemPickupSprite;
        m_pGemPickupSprite = nullptr;
    }

    if (m_pOxygenPickupSprite) {
        delete m_pOxygenPickupSprite;
        m_pOxygenPickupSprite = nullptr;
    }

    m_particleSystems.clear();
    
    m_soundSystem.Release();

}

void SceneMain::OnEnter(bool reset) {

    m_paused = false;
    m_soundSystem.PlaySound("cavebgm");
    m_lightEventTimer = 0.0f;
    m_lightEventSkipped = false;

    if (reset) Reset();
}

void SceneMain::OnExit()
{
    m_soundSystem.StopSound("cavebgm");
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
                    spider->ApplyPushback(pushDirection);
                    m_soundSystem.PlaySound("spiderHit");

                    m_pPlayer->SetCurrentHealth(m_pPlayer->GetCurrentHealth() - 1.0f);
                }
            }
        }
    }
}

bool SceneMain::Initialise(Renderer& renderer)
{
    LogManager::GetInstance().Log("SceneMain is Initialising!");

    m_lightEventInterval = 69.0f;
    m_lightEventTimer = 0.0f;
    m_pVisionLevel = 1;
    m_lightOn = true;
    m_visionLevels = { 1.0f, 1.2f, 1.3f, 1.5f, 2.0f };

    m_dirtParticleCooldown = 1.2f;
    if (!m_soundSystem.Initialise()) {
        std::cerr << "Failed to initialise FMOD system!" << std::endl;
        return false;
    }
    m_soundSystem.LoadSound("waterdrop", "../assets/sound/waterDrop.wav");
    m_soundSystem.LoadSound("pickaxeHit", "../assets/sound/pickaxeHit.wav");
    m_soundSystem.LoadSound("blockBreak", "../assets/sound/blockBreak.wav");
    m_soundSystem.LoadSound("cavebgm", "../assets/sound/cavebgm.mp3");
    m_soundSystem.LoadSound("lightflicker", "../assets/sound/lightFlicker.wav");
    m_soundSystem.LoadSound("spiderHit", "../assets/sound/spiderHit.wav");
    m_soundSystem.LoadSound("playerEat", "../assets/sound/playerEat.wav");

    m_pRenderer = &renderer;
    m_screenWidth = static_cast<float>(renderer.GetWidth());
    m_screenHeight = static_cast<float>(renderer.GetHeight());

    m_pMineBackground = renderer.CreateSprite("../assets/background.png");
   
    m_pVignetteSprite = renderer.CreateSprite("../assets/vignette.png");
    m_pVignetteSprite->SetScale(m_pVisionLevel);

    m_pDarkVignetteSprite = renderer.CreateSprite("../assets/darkVignette.png");
    m_pDarkVignetteSprite->SetScale(m_pVisionLevel);


    float scaleX = static_cast<float>(renderer.GetWidth()) / m_pMineBackground->GetWidth();
    float scaleY = static_cast<float>(renderer.GetHeight()) / m_pMineBackground->GetHeight();
    float scale = std::max(scaleX, scaleY);  //ensuring background covers whole screen

    m_pMineBackground->SetX(renderer.GetWidth() / 2);
    float scaledHeight = m_pMineBackground->GetHeight() * scale;
    m_pMineBackground->SetY(static_cast<int>(scaledHeight / 2.0f));
    m_pMineBackground->SetScale(scale);

    GridState::GetInstance().CreateGrid(renderer, scaledHeight, true);

    m_pPlayer = new Player();
    if (!m_pPlayer->Initialise(renderer))
    {
        delete m_pPlayer;
        m_pPlayer = nullptr;
        return false;
    }
	GridState::GetInstance().SetPlayer(m_pPlayer);

    // Give resources just for testing purposes
    m_pPlayer->AddResource(ResourceType::DIRT, 500);
    m_pPlayer->AddResource(ResourceType::STONE, 500);
    m_pPlayer->AddResource(ResourceType::GEM, 500);

    m_tileSize = GridState::GetInstance().GetTileSize();

    ui = std::make_unique<UI>(&renderer);
    m_screenX = renderer.GetWidth();
    m_playerY = static_cast<float>(m_pPlayer->GetPosition().y);

    m_upgradeManager.Initialise(m_pPlayer);
    // Upgrade station position (to be interacted with menu) also press 'E'
    m_upgradeStations.emplace_back(static_cast<float>(m_pRenderer->GetWidth())*0.15f, static_cast<float>(m_pRenderer->GetHeight()) * 0.9f, 100.0f); // X, y, radius 

    //init particles
    m_pCoinSprite = renderer.CreateSprite("../assets/ball.png");
    m_pCoinSprite->SetScale(0.05f);

    m_pDirtSprite = renderer.CreateSprite("../assets/dirtparticle.png");
    m_pBreakBlockSprite = renderer.CreateSprite("../assets/dirtparticle.png");
    m_pWaterDropSprite = renderer.CreateSprite("../assets/particle.png");

    m_pDirtPickupSprite = renderer.CreateSprite("../assets/dirt_icon.png");
    m_pStonePickupSprite = renderer.CreateSprite("../assets/stone_icon.png");
    m_pGemPickupSprite = renderer.CreateSprite("../assets/gem_icon.png");
    m_pOxygenPickupSprite = renderer.CreateSprite("../assets/bubble.png");

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

    m_timer += deltaTime;

    m_collisionTree->clear();

    // Pause menu & upgrade menu closing logic
    ButtonState escapeState = inputSystem.GetKeyState(SDL_SCANCODE_ESCAPE);
    ButtonState xboxBackState = BS_NEUTRAL;

    if (inputSystem.GetNumberOfControllersAttached() > 0) {
        xboxBackState = inputSystem.GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_BACK);
    }

    if (escapeState == BS_PRESSED || xboxBackState == BS_PRESSED)
    {
        m_paused = true;
        std::cout << "Escape pressed" << std::endl;
        Game::GetInstance().SetCurrentScene(0);
    }

    // To close the Upgrade menu if its open, use ESC to close it
    if (escapeState == BS_PRESSED || xboxBackState == BS_PRESSED)
    {
        if (m_upgradeManager.IsMenuOpen())
        {
            m_upgradeManager.CloseMenu();
            m_pActiveUpgradeStation = nullptr;
            m_isUpgradeMenuUIVisible = false; // Hide ImGui window
            Game::GetInstance().m_pInputSystem->ShowMouseCursor(false); // Hide mouse if game needs it
        }
        else
        {
            m_paused = true;
            Game::GetInstance().SetCurrentScene(7); // 7 is pause screen
            return;
        }
    }

    if (m_paused) {
        return;
    }

	// Sets the vision level based on the player's headlamp level
    if (m_pPlayer && (int)m_pVisionLevel != m_pPlayer->GetHeadlampLevel())
    {
        SetVisionLevel(m_pPlayer->GetHeadlampLevel());
    }

    if (m_pPlayer)
    {
        if (!m_upgradeManager.IsMenuOpen() ||
            !ImGui::GetIO().WantCaptureKeyboard)
        {
            m_pPlayer->Process(deltaTime, inputSystem);
        }
        m_pPlayer->SetDepth(static_cast<int>((m_pPlayer->GetPosition().y / m_tileSize) - m_aboveGroundOffset));

        if (m_godMode) {
            m_pPlayer->SetCurrentHealth(1000.0f);
            m_pPlayer->SetCurrentStamina(1000.0f);
            m_pPlayer->AddOxygen(100.0f);
        }

		if (m_pPlayer->IsInfiniteResources()) {
			m_pPlayer->SetDirt(9999);
			m_pPlayer->SetStone(9999);
			m_pPlayer->SetGem(9999);
		}
    }
    LightEvent(deltaTime);
    DebugFunctions(inputSystem);


    GridState::GetInstance().IsPlayerTouchingFood(m_pPlayer->GetPosition(), static_cast<float>(m_pMineBackground->GetWidth()), static_cast<float>(m_pMineBackground->GetHeight()), static_cast<float>(m_pRenderer->GetHeight()));
    float pX = m_pPlayer->GetPosition().x;
    float pY = m_pPlayer->GetPosition().y;
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
    case 3:
        pickupSprite = m_pOxygenPickupSprite;
    default:

        break;
    }
    //if player is eating
    if (GridState::GetInstance().CheckFood()) {
        float m_munchInterval = 0.5f;
        m_munchTimer += deltaTime;


        if (m_munchTimer >= m_munchInterval) {
            m_munching = false;
            m_munchTimer = 0.0f;
        }
        if (!m_munching) {
            m_soundSystem.PlaySound("playerEat");
            m_munching = true;
        }

    }


    //if player is digging an actual block

    m_dirtParticleCooldown -= deltaTime;
    if (GridState::GetInstance().CheckBlockDig() && m_dirtParticleCooldown <= 0.0f) {

        m_soundSystem.PlaySound("pickaxeHit");
        ParticleSystem ps;
        ps.Initialise(m_pDirtSprite, m_pPlayer, 3, ParticleType::DigDirt);
        ps.ActivateAt(pPos);
        m_particleSystems.push_back(std::move(ps));
        m_dirtParticleCooldown = 0.6f;
    }

    //if player breaks a block

    if (GridState::GetInstance().CheckBlockBreak()) {
        m_pPlayer->SetCurrentStamina(m_pPlayer->GetCurrentStamina() - m_pPlayer->GetStaminaCost());

        m_soundSystem.PlaySound("blockBreak");
        ParticleSystem ps;
        ps.Initialise(m_pBreakBlockSprite, m_pPlayer, 35, ParticleType::BlockBreak);
        ps.ActivateAt(pPos);
        m_particleSystems.push_back(std::move(ps));

        ParticleSystem ps2;
        ps2.Initialise(pickupSprite, m_pPlayer, 5);
        ps2.ActivateAt(m_pPlayer->GetPosition());
        m_particleSystems.push_back(std::move(ps2));
    }
    if (!m_pPlayer->IsPlayerMining()) {
        m_soundSystem.StopSound("pickaxeHit");
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

    // Upgrade station
    m_showUpgradePrompt = false;
    if (!m_upgradeManager.IsMenuOpen())
    {
        for (auto& station : m_upgradeStations)
        {
            if (station.IsPlayerInRange(m_pPlayer->GetPosition(), static_cast<float>(m_pMineBackground->GetWidth()), static_cast<float>(m_pMineBackground->GetHeight()), static_cast<float>(m_pRenderer->GetHeight())))
            {
                m_showUpgradePrompt = true;

                    m_upgradeManager.OpenMenu();
                    m_pActiveUpgradeStation = &station;
                    m_isUpgradeMenuUIVisible = true;
                    Game::GetInstance().m_pInputSystem->ShowMouseCursor(true);
                    break;

            }
        }
    }
    else
    {
        if (m_pActiveUpgradeStation && !m_pActiveUpgradeStation->IsPlayerInRange(m_pPlayer->GetPosition(), static_cast<float>(m_pMineBackground->GetWidth()), static_cast<float>(m_pMineBackground->GetHeight()), static_cast<float>(m_pRenderer->GetHeight())))
        {
            m_upgradeManager.CloseMenu();
            m_pActiveUpgradeStation = nullptr;
            m_isUpgradeMenuUIVisible = false; // Hide ImGui window
            Game::GetInstance().m_pInputSystem->ShowMouseCursor(false); // Hide mouse if game needs it
        }
    }

    m_pVignetteSprite->SetX(static_cast<int>(m_pPlayer->GetPosition().x));
    m_pVignetteSprite->SetY(static_cast<int>(m_pPlayer->GetPosition().y));
    m_pDarkVignetteSprite->SetX(static_cast<int>(m_pPlayer->GetPosition().x));
    m_pDarkVignetteSprite->SetY(static_cast<int>(m_pPlayer->GetPosition().y));

    ui->Update(m_pPlayer, m_pRenderer, deltaTime);

    ProcessParticles(deltaTime);

    m_soundSystem.Update();
}

void SceneMain::Draw(Renderer& renderer)
{

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

    if (m_pVignetteSprite && m_pDarkVignetteSprite) {
        if (m_lightOn == true) {
            m_pVignetteSprite->Draw(renderer);
        }
        else {
            m_pDarkVignetteSprite->Draw(renderer);
        }

    }


    // Upgrade Menu 
    if (m_isUpgradeMenuUIVisible && m_upgradeManager.IsMenuOpen()) 
    {

        if (renderer.GetHeight() < 800) {
            ImGui::SetNextWindowPos(ImVec2(static_cast<float>(m_pRenderer->GetWidth()) * 0.00f, static_cast<float>(m_pRenderer->GetHeight()) * 0.28f), ImGuiCond_Always);  // fixed position
            ImGui::SetNextWindowSize(ImVec2(static_cast<float>(m_pRenderer->GetWidth()) * 0.27f, static_cast<float>(m_pRenderer->GetWidth()) * 0.2f), ImGuiCond_Always); // fixed size

        }
        else {
            ImGui::SetNextWindowPos(ImVec2(static_cast<float>(m_pRenderer->GetWidth()) * 0.07f, static_cast<float>(m_pRenderer->GetHeight()) * 0.24f), ImGuiCond_Always);  // fixed position
            ImGui::SetNextWindowSize(ImVec2(static_cast<float>(m_pRenderer->GetWidth()) * 0.18f, static_cast<float>(m_pRenderer->GetWidth()) * 0.15f), ImGuiCond_Always); // fixed size
        }

        ImGui::Begin("Upgrade Station", &m_isUpgradeMenuUIVisible,
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse
        );

        if (!m_isUpgradeMenuUIVisible) 
        { // If closed by 'X'
            m_upgradeManager.CloseMenu();
            m_pActiveUpgradeStation = nullptr;
            Game::GetInstance().m_pInputSystem->ShowMouseCursor(false);
        }

        const auto& upgrades = m_upgradeManager.GetAvailableUpgrades();
        for (size_t i = 0; i < upgrades.size(); ++i) 
        {
            const auto& upg = upgrades[i];
            std::string buttonLabel = upg.displayName;
            UpgradeCost cost = { ResourceType::DIRT, 0 }; // Default cost for display

            if (upg.IsMaxLevel()) 
            {
                buttonLabel += " - MAX Level (" + std::to_string(upg.currentLevel) + ")";
            }
            else 
            {
                cost = upg.getCostForLevel(upg.currentLevel + 1);
                buttonLabel += " Lv." + std::to_string(upg.currentLevel)
                    + " -> Lv." + std::to_string(upg.currentLevel + 1)
                    + " (Cost: " + std::to_string(cost.amount)
                    + " " + ResourceTypeToString(cost.resource) + ")";
            }

            if (ImGui::Button(buttonLabel.c_str(), ImVec2(-1, 0))) 
            { 
                if (!upg.IsMaxLevel()) 
                {
                    m_upgradeManager.AttemptUpgrade(i);
                }
            }
            // Display description
            if (upg.getDescription) 
            {
                ImGui::TextWrapped("%s", upg.getDescription(upg.currentLevel, cost.amount, ResourceTypeToString(cost.resource)).c_str());
            }
            if (i < upgrades.size() - 1) ImGui::Separator();
        }

        if (!m_upgradeManager.GetStatusMessage().empty()) 
        {
            ImGui::Separator();
            ImGui::TextWrapped("Status: %s", m_upgradeManager.GetStatusMessage().c_str());
        }

        ImGui::Separator();

        ImGui::End();
    }

    ui->Render(); // Draw Last
}


void SceneMain::DebugDraw()
{
    if (m_pPlayer)
    {
        ImGui::NewLine();

        ImGui::Text("Debugging Tools:");
        
        bool godMode = m_godMode;
        if (ImGui::Checkbox("God Mode (F1)", &godMode))
        {
            ToggleGodMode();
        }

        bool infiniteResources = m_pPlayer->IsInfiniteResources();
        if (ImGui::Checkbox("Infinite Resources (F2)", &infiniteResources))
        {
            m_pPlayer->SetInfiniteResources(infiniteResources);
        }

        if (ImGui::Button("Unlock All Upgrades (F3)"))
        {
            // TODO: Implement upgrade unlocking logic
        }

        bool instantMine = m_pPlayer->IsInstantMine();
        if (ImGui::Checkbox("Instant Mine (F4)", &instantMine))
        {
            m_pPlayer->SetInstantMine(instantMine);
        }

        bool noclip = m_pPlayer->IsNoClip();
        if (ImGui::Checkbox("NoClip (F5)", &noclip))
        {
            m_pPlayer->SetNoClip(noclip);
        }

        ImGui::NewLine();
        ImGui::Separator();
        ImGui::NewLine();

        ImGui::Text("Player Information");
        ImGui::Text("Player Coords: %.2f, %.2f", m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y);
        ImGui::Text("Player Health: %.1f health", m_pPlayer->GetCurrentHealth());

        ImGui::NewLine();
        ImGui::Separator();
        ImGui::NewLine();

        ImGui::Text("Grid Information");
        ImGui::Text("Last Broken Block Coords: %.2f, %.2f", GridState::GetInstance().GetBrokenBlockPos().x, GridState::GetInstance().GetBrokenBlockPos().y);
        ImGui::Text("Last Broken Block Grid Coords: %.0f, %.0f", GridState::GetInstance().GetBrokenBlockGridPos().x, GridState::GetInstance().GetBrokenBlockGridPos().y);

        ImGui::NewLine();
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

void SceneMain::DebugFunctions(InputSystem& inputSystem) {
	// Toggle God Mode
    if (inputSystem.GetKeyState(SDL_SCANCODE_F1) == BS_PRESSED)
    {
        ToggleGodMode();
    }

    // Toggle Infinite Resources
    if (inputSystem.GetKeyState(SDL_SCANCODE_F2) == BS_PRESSED)
    {
		m_pPlayer->SetInfiniteResources(!m_pPlayer->IsInfiniteResources());
    }

    // Unlock All Upgrades
    if (inputSystem.GetKeyState(SDL_SCANCODE_F3) == BS_PRESSED)
    {
		// TODO:
    }

    // Instant Mining
    if (inputSystem.GetKeyState(SDL_SCANCODE_F4) == BS_PRESSED)
    {
		m_pPlayer->SetInstantMine(!m_pPlayer->IsInstantMine());
    }

    // Toggle NoClip
    if (inputSystem.GetKeyState(SDL_SCANCODE_F5) == BS_PRESSED)
    {
		m_pPlayer->SetNoClip(!m_pPlayer->IsNoClip());
    }



    // Test particles
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
    if (inputSystem.GetKeyState(SDL_SCANCODE_B) == BS_PRESSED)
    {
        m_lightEventTimer += 100.0f;
        m_lightEventSkipped = true;

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

void SceneMain::SetVisionLevel(int level) 
{
    if (level > 0 && level <= (int)m_visionLevels.size())
    {
        m_pVisionLevel = (float)level;
        float newScale = m_visionLevels[level - 1];
        m_pVignetteSprite->SetScale(newScale);
        m_pDarkVignetteSprite->SetScale(newScale);
    }
}

void SceneMain::LightEvent(float time) {
    m_lightEventTimer += time;

    if (!m_lightOn) {
        m_flicktimer += time;

    }


    if (m_lightEventTimer >= m_lightEventInterval) {


        m_soundSystem.PlaySound("lightflicker");
        m_lightOn = false;
        m_lightEventTimer = 0.0f;
        m_lightEventInterval = 90.0f + static_cast<float>(rand()) / RAND_MAX * 45.0f;
        m_lightEventSkipped = false;
    }

    if (m_flicktimer >= 3.0f) {
        m_lightOn = true;
        m_flicktimer = 0.0f;
    }
}

bool SceneMain::GameWon()
{
    bool won = m_pPlayer->IsAtBottom();

    if (won) Reset();

    return won;
}

void SceneMain::Reset()
{
    CleanUp();

    Reinitialise();
}

void SceneMain::CleanUp()
{
    GridState::GetInstance().ResetGrid();

    m_particleSystems.clear();
}

void SceneMain::Reinitialise()
{
    LogManager::GetInstance().Log("Resetting SceneMain...");


    float scaleX = static_cast<float>(m_pRenderer->GetWidth()) / m_pMineBackground->GetWidth();
    float scaleY = static_cast<float>(m_pRenderer->GetHeight()) / m_pMineBackground->GetHeight();
    float scale = std::max(scaleX, scaleY);
    float scaledHeight = m_pMineBackground->GetHeight() * scale;

    GridState::GetInstance().CreateGrid(*m_pRenderer, scaledHeight, true);

    m_lightEventInterval = 69.0f;
    m_lightEventTimer = 0.0f;
    m_pVisionLevel = 1;
    m_lightOn = true;
    m_visionLevels = { 1.0f, 1.2f, 1.3f, 1.5f, 2.0f };
    m_dirtParticleCooldown = 1.2f;

    m_pVignetteSprite->SetScale(m_pVisionLevel);
    m_pDarkVignetteSprite->SetScale(m_pVisionLevel);

    m_pPlayer->Reset();

    m_playerY = static_cast<float>(m_pPlayer->GetPosition().y);

    m_pRenderer->SetCameraPosition(static_cast<float>(m_screenX / 2), m_pMineBackground->GetHeight() * 0.1f);

    for (size_t i = 0; i < m_testSpider->totalCount(); i++) {
        if (GameObject* obj = m_testSpider->getObjectAtIndex(i)) {
            SpiderState* spider = dynamic_cast<SpiderState*>(obj);
            spider->SetActive(false);
        }
    }

    LogManager::GetInstance().Log("SceneMain Reinitialization complete");
}

bool SceneMain::IsFinished()
{
    return m_gameOver;
}
