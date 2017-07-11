#include "ActionZVZZerglingMutalisk.h"
#include "BuildingManager.h"
#include "ActionHelper.h"
#include "InformationManager.h"

using namespace CasiaBot;
using namespace UAlbertaBot;
using namespace CasiaBot::ActionHelper;
using namespace std;

ActionZVZZerglingMutalisk::ActionZVZZerglingMutalisk()
{
	init();
}

void ActionZVZZerglingMutalisk::init()
{
	if (!isInitialized)
	{
		updateCurrentState();
		mineralNetIncrease = { 0,0,0,0,0 };
		gasNetIncrease = { 0,0,0,0,0 };
		lastFrameCount = BWAPI::Broodwar->getFrameCount();
		lastFrameMineralAmount = BWAPI::Broodwar->self()->minerals();
		lastFrameGasAmount = BWAPI::Broodwar->self()->gas();
		isInitialized = true;
	}
}

bool ActionZVZZerglingMutalisk::canDeployAction()
{
	updateState();
	if (enemy_mutalisk_count > 0 || (enemy_hydralisk_count == 0 && enemy_mutalisk_count == 0))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ActionZVZZerglingMutalisk::tick()
{
	updateState();
	if (enemy_hydralisk_count > 0 && enemy_mutalisk_count == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ActionZVZZerglingMutalisk::getBuildOrderList(UAlbertaBot::ProductionQueue & queue)
{
	updateState();

	// 判断是否需要增加母巢
	int currentFrameCount = BWAPI::Broodwar->getFrameCount();
	if (hatch_count <= 4 && currentFrameCount && currentFrameCount % 200 == 0)
	{
		int currentFrameMineralAmount = BWAPI::Broodwar->self()->minerals();
		int currentFrameGasAmount = BWAPI::Broodwar->self()->gas();
		int diffMineralAmount = currentFrameMineralAmount - lastFrameMineralAmount;
		int diffGasAmount = currentFrameGasAmount - lastFrameGasAmount;

		mineralNetIncrease.pop_front();
		mineralNetIncrease.push_back(diffMineralAmount);
		gasNetIncrease.pop_front();
		gasNetIncrease.push_back(diffGasAmount);

		bool mineralDequePositive;
		bool gasDequePositive;

		if (hatch_count <= 2)
		{
			mineralDequePositive = IsDequeAllPositive(mineralNetIncrease);
			if (mineralDequePositive)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hatchery), true);
			}
		}
		else
		{
			mineralDequePositive = IsDequeAllPositive(mineralNetIncrease);
			gasDequePositive = IsDequeAllPositive(gasNetIncrease);
			if (mineralDequePositive && gasDequePositive)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hatchery), true);
			}
		}

		lastFrameCount = currentFrameCount;
		lastFrameMineralAmount = currentFrameMineralAmount;
		lastFrameGasAmount = currentFrameGasAmount;
	}

	// 判断前提建筑是否存在
	bool isExtractorExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Extractor);
	if (!isExtractorExist)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Extractor), true);
	}

	bool isSpawningPoolExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Spawning_Pool);
	if (!isSpawningPoolExist)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spawning_Pool), true);
	}

	bool isLairExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Lair);
	if (!isLairExist)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Lair), true);
	}

	bool isSpireExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Spire);
	if (!isSpireExist)	// 若飞龙塔不存在
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spire), true);
	}

	queue.add(MetaType(BWAPI::UpgradeTypes::Metabolic_Boost), true);
	queue.add(MetaType(BWAPI::UpgradeTypes::Adrenal_Glands), true);

	// 判断人口是否接近满，系统内部的supply数值是双倍的
	if (BWAPI::Broodwar->self()->supplyUsed() + 8 >= BWAPI::Broodwar->self()->supplyTotal())
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Overlord), true);
	}

	// 判断需要建造多少部队
	int need_zergling_count = (int)(enemy_zergling_count * 1.5) - zergling_count;
	int need_mutalisk_count = (int)(enemy_zergling_count*0.5) + enemy_mutalisk_count + enemy_hydralisk_count;

	// 穿插建造Zergling和Mutalisk
	do
	{
		if (need_zergling_count > 0)
		{
			// 2个Zergling
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Zergling));
			need_zergling_count -= 2;
		}
		if (need_mutalisk_count > 0)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Mutalisk));
			need_mutalisk_count--;
		}
		if (need_zergling_count <= 0 && need_mutalisk_count <= 0)
		{
			break;
		}

	} while (true);

	int extractorUpperBound = std::min(hatch_count, 2);
	int currentExtractorCount = (int)InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Extractor, BWAPI::Broodwar->self());
	if (currentExtractorCount < extractorUpperBound)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Extractor), true);
	}
}

void ActionZVZZerglingMutalisk::updateState()
{
	updateCurrentState();

	auto &info = InformationManager::Instance();
	auto self = BWAPI::Broodwar->self();
	auto enemy = BWAPI::Broodwar->enemy();
	hatch_count = info.getNumUnits(BWAPI::UnitTypes::Zerg_Hatchery, self) + info.getNumUnits(BWAPI::UnitTypes::Zerg_Lair, self) + info.getNumUnits(BWAPI::UnitTypes::Zerg_Hive, self);
	zergling_count = info.getNumUnits(BWAPI::UnitTypes::Zerg_Zergling, self);
	mutalisk_count = info.getNumUnits(BWAPI::UnitTypes::Zerg_Mutalisk, self);
	enemy_zergling_count = info.getNumUnits(BWAPI::UnitTypes::Zerg_Zergling, enemy);
	enemy_hydralisk_count = info.getNumUnits(BWAPI::UnitTypes::Zerg_Hydralisk, enemy);
	enemy_mutalisk_count = info.getNumUnits(BWAPI::UnitTypes::Zerg_Mutalisk, enemy);
}
