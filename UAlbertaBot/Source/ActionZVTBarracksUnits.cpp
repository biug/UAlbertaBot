#include "ActionZVTBarracksUnits.h"
#include "BuildingManager.h"
#include "ActionHelper.h"
#include "InformationManager.h"

using namespace CasiaBot;
using namespace UAlbertaBot;
using namespace CasiaBot::ActionHelper;
using namespace std;

ActionZVTBarracksUnits::ActionZVTBarracksUnits()
{
	init();
}

void ActionZVTBarracksUnits::init()
{
	if (!isInitialized)
	{
		mineralNetIncrease = { 0,0,0,0,0 };
		gasNetIncrease = { 0,0,0,0,0 };
		lastFrameCount = BWAPI::Broodwar->getFrameCount();
		lastFrameMineralAmount = BWAPI::Broodwar->self()->minerals();
		lastFrameGasAmount = BWAPI::Broodwar->self()->gas();
		isInitialized = true;
	}
}

bool ActionZVTBarracksUnits::canDeployAction()
{
	if (enemyTerranMechanizationRate < 1)
	{
		return true;
	}
	else
		return false;
}

bool ActionZVTBarracksUnits::tick()
{
	if (enemyTerranMechanizationRate >= 1)
	{
		return true;
	}
	else
		return false;
}

void ActionZVTBarracksUnits::getBuildOrderList(UAlbertaBot::ProductionQueue & queue)
{
	// 判断是否需要增加母巢
	int drone_count_exist = InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Drone, BWAPI::Broodwar->self());
	int currentFrameCount = BWAPI::Broodwar->getFrameCount();
	if (hatch_count <= 4 && currentFrameCount > 10 && currentFrameCount % 200 == 0)
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
	bool isExtractorExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Extractor) || extractor_count > 0;
	if (!isExtractorExist && drone_count_exist >= 7 && InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Spawning_Pool, BWAPI::Broodwar->self()))
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone));
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Extractor), true);
	}

	bool isSpawningPoolExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Spawning_Pool) || spawning_pool_count > 0;
	if (!isSpawningPoolExist)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone));
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spawning_Pool), true);
	}

	bool isHydraliskDenExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Hydralisk_Den) || hydralisk_den_count > 0;
	if (!isHydraliskDenExist)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone));
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hydralisk_Den), true);
	}

	bool isHiveExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Hive) || hive_count > 0;
	bool isQueenNestExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Queens_Nest) || queens_nest_count > 0;
	bool isLairExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Lair) || lair_count > 0;
	if (!isLairExist)
	{
		if (currentFrameCount > 4800)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Lair));
		}
	}

	if (InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Spawning_Pool, BWAPI::Broodwar->self()) > 0 && queue.upgradeCount(BWAPI::UpgradeTypes::Metabolic_Boost) == 0)
	{
		queue.add(MetaType(BWAPI::UpgradeTypes::Metabolic_Boost));
	}
	if (InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Hydralisk_Den, BWAPI::Broodwar->self()) > 0 && InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Lair, BWAPI::Broodwar->self()) > 0 && queue.techCount(BWAPI::TechTypes::Lurker_Aspect) == 0)
	{
		queue.add(MetaType(BWAPI::TechTypes::Lurker_Aspect));
	}
	if (InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Hive, BWAPI::Broodwar->self()) > 0 && queue.upgradeCount(BWAPI::UpgradeTypes::Adrenal_Glands) == 0)
	{
		queue.add(MetaType(BWAPI::UpgradeTypes::Adrenal_Glands));
	}

	// 判断需要建造多少部队
	int need_zergling_count = (int)(enemyTerranBarrackUnitsAmount * 1.5) - zergling_count;
	if (need_zergling_count <= 0 && zergling_count < 24)
	{
		need_zergling_count = 2;
	}
	int need_lurker_count = std::min(1, need_zergling_count / 3);

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
			if (InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Hydralisk_Den, BWAPI::Broodwar->self()) > 0)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hydralisk));
			}
			if (BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Lurker_Aspect)
				&& InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Hydralisk, BWAPI::Broodwar->self()) > 0)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Lurker));
			}
			need_lurker_count--;
		}
		if (need_zergling_count <= 0 && need_lurker_count <= 0)
		{
			break;
		}

	} while (true);

	if (drone_count * 15 < hatch_count)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone));
	}

	//int extractorUpperBound = std::min(hatch_count, 3);
	//int currentExtractorCount = (int)InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Extractor, BWAPI::Broodwar->self());
	//if (currentExtractorCount < extractorUpperBound)
	//{
	//	queue.add(MetaType(BWAPI::UnitTypes::Zerg_Extractor));
	//}

}

void ActionZVTBarracksUnits::updateCurrentState(ProductionQueue &queue)
{
	ActionZergBase::updateCurrentState(queue);

	enemyTerranBarrackUnitsAmount = enemy_marine_count + enemy_firebat_count + enemy_ghost_count + enemy_medic_count;
	enemyTerranFactoryUnitsAmount = enemy_vulture_count + enemy_tank_count + enemy_goliath_count;
	enemyTerranMechanizationRate = enemyTerranBarrackUnitsAmount == 0 ? 10 : (double)enemyTerranFactoryUnitsAmount / (double)enemyTerranBarrackUnitsAmount;
}

