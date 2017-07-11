#include "ActionZVZZerglingLurker.h"
#include "BuildingManager.h"
#include "ActionHelper.h"
#include "InformationManager.h"

using namespace CasiaBot;
using namespace UAlbertaBot;
using namespace CasiaBot::ActionHelper;
using namespace std;

ActionZVZZerglingLurker::ActionZVZZerglingLurker()
{
	init();
}

void ActionZVZZerglingLurker::init()
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

bool ActionZVZZerglingLurker::canDeployAction()
{
	updateState();
	if (enemy_mutalisk_count <= 0 && enemy_hydralisk_count > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ActionZVZZerglingLurker::tick()
{
	updateState();
	if (enemy_hydralisk_count <= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ActionZVZZerglingLurker::getBuildOrderList(UAlbertaBot::ProductionQueue & queue)
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

	bool isHydraliskDenExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
	if (!isHydraliskDenExist)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hydralisk_Den), true);
	}

	bool isLairExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Lair);
	if (!isLairExist)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Lair), true);
	}

	queue.add(MetaType(BWAPI::TechTypes::Lurker_Aspect), true);
	queue.add(MetaType(BWAPI::UpgradeTypes::Metabolic_Boost), true);
	queue.add(MetaType(BWAPI::UpgradeTypes::Adrenal_Glands), true);

	// 判断人口是否接近满，系统内部的supply数值是双倍的
	if (BWAPI::Broodwar->self()->supplyUsed() + 8 >= BWAPI::Broodwar->self()->supplyTotal())
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Overlord), true);
	}

	// 判断需要建造多少部队
	int need_zergling_count = (int)(enemy_zergling_count * 1.7) - zergling_count;
	int need_lurker_count = (int)(enemy_hydralisk_count * 1.2);

	for (int i = 0; i < hydralisk_count; i++)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Lurker), true);
	}

	// 穿插建造Zergling和Lurker
	do
	{
		if (need_zergling_count > 0)
		{
			// 2个Zergling
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Zergling));
			need_zergling_count -= 2;
		}
		if (need_lurker_count > 0)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hydralisk));
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Lurker));
			need_lurker_count--;
		}
		if (need_zergling_count <= 0 && need_lurker_count <= 0)
		{
			break;
		}

	} while (true);

	int extractorUpperBound = std::min(hatch_count, 3);
	int currentExtractorCount = (int)InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Extractor, BWAPI::Broodwar->self());
	if (currentExtractorCount < extractorUpperBound)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Extractor));
	}
}

void ActionZVZZerglingLurker::updateState()
{
	updateCurrentState();

	auto &info = InformationManager::Instance();
	auto self = BWAPI::Broodwar->self();
	auto enemy = BWAPI::Broodwar->enemy();
	hatch_count = info.getNumUnits(BWAPI::UnitTypes::Zerg_Hatchery, self) + info.getNumUnits(BWAPI::UnitTypes::Zerg_Lair, self) + info.getNumUnits(BWAPI::UnitTypes::Zerg_Hive, self);
	zergling_count = info.getNumUnits(BWAPI::UnitTypes::Zerg_Zergling, self);
	hydralisk_count = info.getNumUnits(BWAPI::UnitTypes::Zerg_Hydralisk, self);
	lurker_count = info.getNumUnits(BWAPI::UnitTypes::Zerg_Lurker, self);
	enemy_zergling_count = info.getNumUnits(BWAPI::UnitTypes::Zerg_Zergling, enemy);
	enemy_hydralisk_count = info.getNumUnits(BWAPI::UnitTypes::Zerg_Hydralisk, enemy);
	enemy_mutalisk_count = info.getNumUnits(BWAPI::UnitTypes::Zerg_Mutalisk, enemy);
}
