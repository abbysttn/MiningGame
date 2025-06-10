#ifndef __UPGRADEMANAGER_H__
#define __UPGRADEMANAGER_H__

// Lib
#include <vector>
#include <string>
#include <functional>

// Local
#include "player.h"
#include "resourcetype.h"

struct UpgradeCost
{
	ResourceType resource;
	int amount;
};

struct Upgrade
{
	std::string id;
	std::string displayName;
	int currentLevel;
	int maxLevel;

	std::function<UpgradeCost(int targetLevel)> getCostForLevel;

	std::function<void(Player& player, int newLevel)> applyEffect;

	std::function<std::string(int currentLevel, int nextLevelCostAmount, const std::string& nextLevelCostType)> getDescription;

	Upgrade(std::string uniqueId
		, std::string name
		, int maxLvl
		, std::function<UpgradeCost(int)> costFunc
		, std::function<void(Player&, int)> effectFunc
		, std::function<std::string(int, int, const std::string&)> descFunc = nullptr) 
		: id(std::move(uniqueId))
		, displayName(std::move(name))
		, currentLevel(0), maxLevel(maxLvl)
		, getCostForLevel(std::move(costFunc))
		, applyEffect(std::move(effectFunc))
		, getDescription(std::move(descFunc)) 
	{}

	bool IsMaxLevel() const { return currentLevel >= maxLevel; }
};

class UpgradeManager
{
private:
	Player* m_player;
	std::vector<Upgrade> m_availableUpgrades;
	bool m_isMenuOpen;
	std::string m_statusMessage;

public:
	UpgradeManager();

	void Initialise(Player* player);

	void OpenMenu();
	void CloseMenu();
	bool IsMenuOpen() const { return m_isMenuOpen; }

	void AttemptUpgrade(size_t upgradeIndex);
	void AttemptUpgradeById(const std::string& upgradeId);

	const std::vector<Upgrade>& GetAvailableUpgrades() const { return m_availableUpgrades; }
	Upgrade* GetUpgradeById(const std::string& upgradeId);

	const std::string& GetStatusMessage() const { return m_statusMessage; }
	void ClearStatusMessage() { m_statusMessage.clear(); }

private:
	void DefineUpgrades();
	void SyncUpgradeLevelsWithPlayer();
};

#endif // !__UPGRADEMANAGER_H__
