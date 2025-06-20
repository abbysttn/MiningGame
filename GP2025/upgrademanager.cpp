// This include
#include "upgrademanager.h"

// Lib
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
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

    // Mining Strength Upgrade
    m_availableUpgrades.push_back(Upgrade
    {
        "mining_strength",     // ID
        "Mining Strength",     // Display Name
        5,                     // Max Level
        [](int targetLevel) -> UpgradeCost 
        {
            // How much the next level would cost
            return {ResourceType::STONE, 5 * targetLevel};
        },
        [this](Player& player, int newLevel) 
        {
			// Sets the mining strength level for the player
            player.SetMiningStrengthLevel(newLevel); // Maybe the blocks will have like 'needs miningstr 2 or something'
            m_statusMessage = "Mining Strength upgraded to Lvl " + std::to_string(newLevel) + "!";
        },
        [](int currentLevel, int costAmount, const std::string& costType) -> std::string 
        {
            if (currentLevel == 0) return "Increases mining strength to mine tougher blocks!";
            return "Current Mining Strength: Lvl " + std::to_string(currentLevel) + ". Next level increases mining strength";
        }
	});

    // Max Stamina Upgrade
    m_availableUpgrades.push_back(Upgrade
    {
        "max_stamina",        // ID
        "Max Stamina",        // Display Name
        5,                    // Max Level
        [](int targetLevel) -> UpgradeCost 
        {
            return {ResourceType::DIRT, 10 + (5 * targetLevel)};
        },
        [this](Player& player, int newLevel) 
        {
            float baseMaxStamina = 100.0f;
			float newUpgradedStamina = baseMaxStamina + ((float)newLevel * 50.0f);
            player.SetMaxStamina(newUpgradedStamina);
            player.SetCurrentStamina(player.GetMaxStamina());
            m_statusMessage = "Max Stamina upgraded to " + std::to_string(static_cast<int>(player.GetMaxStamina())) + "!";
        },
        [](int currentLevel, int costAmount, const std::string& costType) -> std::string 
        {
            if (currentLevel == 0) return "Increases your maximum stamina for deeper depths";
            return "Current Max Stamina: " + std::to_string(static_cast<int>(100.0f + currentLevel * 50.0f)) + ". Next level increases max stamina by 50.";
        }
    });

    // Jump Height Upgrade
    m_availableUpgrades.push_back(Upgrade
    {
        "jump_height",        // ID
        "Jump Height",        // Display Name
        5,                    // Max Level
        [](int targetLevel) -> UpgradeCost 
        {
            return {ResourceType::GEM, 10 * targetLevel};
        },
        [this](Player& player, int newLevel) 
        {
            float baseJumpMultiplier = 1.0f;
            player.SetJumpHeightMultiplier(baseJumpMultiplier + ((float)newLevel * 0.1f));
            m_statusMessage = "Jump Height increased!";
        },
        [](int currentLevel, int costAmount, const std::string& costType) -> std::string 
        {
			float currentMultiplier = 1.0f + currentLevel * 0.1f;

            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1);

            if (currentLevel == 0)
            {
                oss << "Allows you to jump higher.";
            }
            else
            {
                oss << "Current Jump Multiplier: " << currentMultiplier << "x. " << "Next level further increases jump height.";
            }
            return oss.str();
        }
    });

	// Headlamp Upgrade
    m_availableUpgrades.push_back(Upgrade
    {
        "headlamp",          // ID
        "Headlamp",          // Display Name
        3,                   // Max Level
        [](int targetLevel) -> UpgradeCost
        {
            // Base cost for first upgrade
            if (targetLevel <= 1) return {ResourceType::GEM, 3};
            return {ResourceType::GEM, 3 + (2 * (targetLevel - 2))};
        },
        [this](Player& player, int newLevel)
        {
            player.SetHeadlampLevel(newLevel + 1);
            m_statusMessage = "Headlamp upgraded to Lvl " + std::to_string(newLevel) + "!";
        },
        [](int currentLevel, int costAmount, const std::string& costType) -> std::string
        {
            float currentVisibility = 1.0f + (currentLevel * 0.15f);
            if (currentLevel == 0) return "Improves your headlamp to see further in the dark.";
            return "Increases your light radius. Next level provides even more visibility.";
        }
    });

    // Health Upgrade
    m_availableUpgrades.push_back(Upgrade
        {
            "max_health",        // ID
            "Max Health",        // Display Name
            5,                   // Max Level
            [](int targetLevel) -> UpgradeCost
            {
                return {ResourceType::DIRT, 10 + (5 * targetLevel)};
            },
            [this](Player& player, int newLevel)
            {
                float baseMaxHealth = 100.0f;
                float newUpgradedHealth = baseMaxHealth + ((float)newLevel * 25.0f);
                player.SetMaxHealth(newUpgradedHealth);
                player.SetCurrentHealth(player.GetMaxHealth());
                m_statusMessage = "Max Health upgraded to " + std::to_string(static_cast<int>(player.GetMaxHealth())) + "!";
            },
            [](int currentLevel, int costAmount, const std::string& costType) -> std::string
            {
                if (currentLevel == 0) return "Increases your maximum health for those pesky spiders!!";
                return "Current Max Health: " + std::to_string(static_cast<int>(100.0f + currentLevel * 25.0f)) + ". Next level increases max health by 25.";
            }
        });
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
        else if (upgrade.id == "headlamp")
        {
            upgrade.currentLevel = m_player->GetHeadlampLevel() - 1;
        }
        else if (upgrade.id == "max_health")
        {
            float baseMaxHealth = 100.0f;
            float incrementPerLevel = 20.0f;
            if (m_player->GetMaxHealth() > baseMaxHealth && incrementPerLevel > 0)
            {
                upgrade.currentLevel = static_cast<int>(round((m_player->GetMaxHealth() - baseMaxHealth) / incrementPerLevel));
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