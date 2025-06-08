// This include
#include "upgrademanager.h"

// Lib
#include <iostream>
#include <cmath>

UpgradeManager::UpgradeManager() 
    : m_player(nullptr)
    , m_isMenuOpen(false) 
{
}

void UpgradeManager::Initialise(Player* player) 
{
    m_player = player;
    if (!m_player) 
    {
        std::cerr << "Error: Player object not provided to UpgradeManager." << std::endl;
        return;
    }
    DefineUpgrades();
    SyncUpgradeLevelsWithPlayer(); // Sync after defining base upgrades
}

void UpgradeManager::DefineUpgrades() 
{
    m_availableUpgrades.clear();

    // --- Mining Strength Upgrade ---
    m_availableUpgrades.emplace_back(
        "mining_strength",     // Unique ID
        "Mining Strength",     // Display Name
        5,                     // Max Level
        [](int targetLevel) -> UpgradeCost 
        { // Cost Function for *targetLevel*
            // Example: Lv1 costs 50, Lv2 costs 100, Lv3 costs 150 stone
            return { ResourceType::STONE, 50 * targetLevel };
        },
        [this](Player& player, int newLevel) 
        { // Effect Function
            player.SetMiningStrengthLevel(newLevel);
            m_statusMessage = "Mining Strength upgraded to Lv" + std::to_string(newLevel) + "!";
        },
        [](int currentLevel, int costAmount, const std::string& costType) -> std::string 
        { // Description
            if (currentLevel == 0) return "Increases mining speed and ability to mine harder blocks.";
            return "Current Mining Strength: Lv" + std::to_string(currentLevel) +
                ". Next level increases mining effectiveness.";
        }
    );

    // --- Max Stamina Upgrade ---
    m_availableUpgrades.emplace_back(
        "max_stamina",        // Unique ID
        "Max Stamina",        // Display Name
        5,                    // Max Level
        [](int targetLevel) -> UpgradeCost 
        { // Cost Function
            // Example: Lv1 costs 75, Lv2 costs 100, Lv3 costs 125 dirt
            return { ResourceType::DIRT, 50 + (25 * targetLevel) };
        },
        [this](Player& player, int newLevel) 
        { // Effect Function
            // Each level adds 20 max stamina from a base of 100
            float baseMaxStamina = 100.0f;
            player.SetMaxStamina(baseMaxStamina + ((float)newLevel * 20.0f));
            // Also restore some stamina on upgrade
            player.SetCurrentStamina(player.GetCurrentStamina() + 20.0f);
            m_statusMessage = "Max Stamina upgraded to " + std::to_string(static_cast<int>(player.GetMaxStamina())) + "!";
        },
        [](int currentLevel, int costAmount, const std::string& costType) -> std::string 
        {
            if (currentLevel == 0) return "Increases your maximum stamina for sustained effort.";
            return "Current Max Stamina: " + std::to_string(static_cast<int>(100.0f + currentLevel * 20.0f)) +
                ". Next level increases max stamina by 20.";
        }
    );

    // --- Jump Height Upgrade ---
    m_availableUpgrades.emplace_back(
        "jump_height",        // Unique ID
        3,                    // Max Level
        [](int targetLevel) -> UpgradeCost 
        { // Cost Function
            // Example: Lv1 costs 10, Lv2 costs 20, Lv3 costs 30 gems
            return { ResourceType::GEM, 10 * targetLevel };
        },
        [this](Player& player, int newLevel) 
        { // Effect Function
            // Each level adds 0.1 to jump multiplier from a base of 1.0
            float baseJumpMultiplier = 1.0f;
            player.SetJumpHeightMultiplier(baseJumpMultiplier + ((float)newLevel * 0.1f));
            m_statusMessage = "Jump Height increased!";
        },
        [](int currentLevel, int costAmount, const std::string& costType) -> std::string 
        {
            if (currentLevel == 0) return "Allows you to jump higher.";
            return "Current Jump Multiplier: " + std::to_string(1.0f + currentLevel * 0.1f) + "x. " +
                "Next level further increases jump height.";
        }
    );
}

void UpgradeManager::SyncUpgradeLevelsWithPlayer() 
{
    if (!m_player) return;

    for (auto& upgrade : m_availableUpgrades) 
    {
        if (upgrade.id == "mining_strength") 
        {
        
            upgrade.currentLevel = m_player->GetMiningStrengthLevel();
        }
        else if (upgrade.id == "max_stamina") 
        {
            float baseMaxStamina = 100.0f;
            float incrementPerLevel = 20.0f;
            if (m_player->GetMaxStamina() >= baseMaxStamina && incrementPerLevel > 0) 
            {
                upgrade.currentLevel = static_cast<int>(round((m_player->GetMaxStamina() - baseMaxStamina) / incrementPerLevel));
            }
            else 
            {
                upgrade.currentLevel = 0;
            }
        }
        else if (upgrade.id == "jump_height") 
        {
            float baseJumpMultiplier = 1.0f;
            float incrementPerLevel = 0.1f;
            if (m_player->GetJumpHeightMultiplier() >= baseJumpMultiplier && incrementPerLevel > 0) 
            {
                upgrade.currentLevel = static_cast<int>(round((m_player->GetJumpHeightMultiplier() - baseJumpMultiplier) / incrementPerLevel));
            }
            else 
            {
                upgrade.currentLevel = 0;
            }
        }
        // Ensure levels are within bounds
        if (upgrade.currentLevel < 0) upgrade.currentLevel = 0;
        if (upgrade.currentLevel > upgrade.maxLevel) upgrade.currentLevel = upgrade.maxLevel;
    }
}

void UpgradeManager::OpenMenu() 
{
    if (!m_player) return;
    m_isMenuOpen = true;
    m_statusMessage.clear();
    SyncUpgradeLevelsWithPlayer(); // Re-sync in case player stats changed externally or for first open
}

void UpgradeManager::CloseMenu() 
{
    m_isMenuOpen = false;
}

Upgrade* UpgradeManager::GetUpgradeById(const std::string& id) 
{
    for (auto& upg : m_availableUpgrades) 
    {
        if (upg.id == id) 
        {
            return &upg;
        }
    }
    return nullptr;
}

void UpgradeManager::AttemptUpgradeById(const std::string& upgradeId) 
{
    for (size_t i = 0; i < m_availableUpgrades.size(); ++i) 
    {
        if (m_availableUpgrades[i].id == upgradeId) 
        {
            AttemptUpgrade(i);
            return;
        }
    }
    m_statusMessage = "Unknown upgrade ID.";
}


void UpgradeManager::AttemptUpgrade(size_t upgradeIndex) 
{
    if (!m_player || !m_isMenuOpen || upgradeIndex >= m_availableUpgrades.size()) 
    {
        m_statusMessage = "Invalid action.";
        return;
    }

    Upgrade& upgrade = m_availableUpgrades[upgradeIndex];

    if (upgrade.IsMaxLevel()) 
    {
        m_statusMessage = upgrade.displayName + " is already at max level!";
        return;
    }

    int targetLevel = upgrade.currentLevel + 1;
    UpgradeCost cost = upgrade.getCostForLevel(targetLevel);

    if (m_player->HasResource(cost.resource, cost.amount)) 
    {
        m_player->RemoveResource(cost.resource, cost.amount);
        upgrade.applyEffect(*m_player, targetLevel);
        upgrade.currentLevel = targetLevel; 
    }
    else 
    {
        m_statusMessage = "Not enough " + ResourceTypeToString(cost.resource) + "! Need " + std::to_string(cost.amount) + ". You have " + std::to_string(m_player->GetResourceCount(cost.resource)) + ".";
    }
}