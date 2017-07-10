#include "ActionZVTFactoriesUnits.h"
#include "BuildingManager.h"
#include "ActionHelper.h"

using namespace CasiaBot;
using namespace UAlbertaBot;
using namespace CasiaBot::ActionHelper;
using namespace std;

ActionZVTFactoriesUnits::ActionZVTFactoriesUnits()
{
	init();
}

void ActionZVTFactoriesUnits::init()
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

bool ActionZVTFactoriesUnits::canDeployAction()
{
	updateState();
	if (enermyTerranMechanizationRate >= 2)
	{
		return true;
	}
	else
		return false;
}

bool ActionZVTFactoriesUnits::tick()
{
	updateState();
	if (enermyTerranMechanizationRate <= 1)
	{
		return true;
	}
	else
		return false;
}

void ActionZVTFactoriesUnits::getBuildOrderList(UAlbertaBot::ProductionQueue & queue)
{
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

	bool isDefilerMoundExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Defiler_Mound);
	bool isSpireExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Spire);
	bool isHiveExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Hive);
	bool isQueenNestExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Queens_Nest);
	bool isLairExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Lair);
	if (isDefilerMoundExist)	// 若蝎子巢存在
	{
		if (!isSpireExist)	// 若飞龙塔不存在
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spire), true);
		}
	}
	else	// 若蝎子巢不存在
	{
		if (isHiveExist)	// 若蜂巢存在
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Defiler_Mound), true);
			if (!isSpireExist)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spire), true);
			}
		}
		else	// // 若蜂巢不存在
		{
			if (isQueenNestExist)	// 若皇后巢存在
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hive), true);
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Defiler_Mound), true);
				if (!isSpireExist)
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spire), true);
				}
			}
			else	// 若皇后巢不存在
			{
				if (isLairExist)	// 若兽穴存在
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Queens_Nest), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hive), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Defiler_Mound), true);
					if (!isSpireExist)
					{
						queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
						queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spire), true);
					}
				}
				else	// 若兽穴不存在
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Lair), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Queens_Nest), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hive), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Defiler_Mound), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spire), true);
				}
			}
		}
	}

	queue.add(MetaType(BWAPI::UpgradeTypes::Metabolic_Boost), true);
	queue.add(MetaType(BWAPI::UpgradeTypes::Adrenal_Glands), true);

	int currentFlyerCarapaceLevel = BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Zerg_Flyer_Carapace);
	bool isFlyerCarapaceUpgrading = BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Zerg_Flyer_Carapace);
	if (!isFlyerCarapaceUpgrading && currentFlyerCarapaceLevel < 1)
	{
		queue.add(MetaType(BWAPI::UpgradeTypes::Zerg_Flyer_Carapace), true);
	}

	// 判断人口是否接近满，系统内部的supply数值是双倍的
	if (BWAPI::Broodwar->self()->supplyUsed() + 8 >= BWAPI::Broodwar->self()->supplyTotal())
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Overlord), true);
	}

	if (enermy_army_supply < escalationMark)
	{
		// 判断需要建造多少部队
		int need_mutalisk_count = (int)(enermy_goliath_count * 1.2 + 2) - mutalisk_count;
		int need_defiler_count = 3;
		int need_zergling_count = enermyTerranFactoryUnitsAmount * 2 - zergling_count;

		// 穿插建造Mutalisk、Defiler、Zergling
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
			if (need_defiler_count > 0)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Defiler));
				need_defiler_count--;
			}
			if (need_mutalisk_count <= 0 && need_defiler_count <= 0 && need_zergling_count <= 0)
			{
				break;
			}

		} while (true);
	}
	else
	{
		bool isUltraliskCavernExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Ultralisk_Cavern);
		if (!isUltraliskCavernExist)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Ultralisk_Cavern), true);
		}

		// 判断需要建造多少部队
		int need_mutalisk_count = (int)(enermy_goliath_count * 1.2 + 2) - mutalisk_count;
		int need_defiler_count = 3;
		int need_zergling_count = enermyTerranFactoryUnitsAmount * 2 - zergling_count;

		// 穿插建造Mutalisk、Defiler、Zergling、Ultralisk
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
			if (need_defiler_count > 0)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Defiler));
				need_defiler_count--;
			}
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Ultralisk));
			if (need_mutalisk_count <= 0 && need_defiler_count <= 0 && need_zergling_count <= 0)
			{
				break;
			}

		} while (true);
	}
}

void ActionZVTFactoriesUnits::updateState()
{
	updateCurrentState();
	enermyTerranBarrackUnitsAmount = enermy_marine_count + enermy_firebat_count + enermy_ghost_count + enermy_medic_count;
	enermyTerranFactoryUnitsAmount = enermy_vulture_count + enermy_tank_count + enermy_goliath_count;
	enermyTerranMechanizationRate = enermyTerranFactoryUnitsAmount / enermyTerranBarrackUnitsAmount;
}
