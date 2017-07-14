#include "ActionZVTFactoriesUnits.h"
#include "BuildingManager.h"
#include "ActionHelper.h"
#include "InformationManager.h"

using namespace CasiaBot;
using namespace CasiaBot::ActionHelper;
using namespace std;

ActionZVTFactoriesUnits::ActionZVTFactoriesUnits()
{
}

void ActionZVTFactoriesUnits::init()
{
	if (!isInitialized)
	{
		lastFrameCount = BWAPI::Broodwar->getFrameCount();
		lastFrameMineralAmount = BWAPI::Broodwar->self()->minerals();
		lastFrameGasAmount = BWAPI::Broodwar->self()->gas();
		isInitialized = true;
	}
}

bool ActionZVTFactoriesUnits::canDeployAction()
{
	if (enemyTerranMechanizationRate >= 0.8)
	{
		return true;
	}
	else
		return false;
}

bool ActionZVTFactoriesUnits::tick()
{
	if (enemyTerranMechanizationRate < 0.8)
	{
		return true;
	}
	else
		return false;
}

void ActionZVTFactoriesUnits::getBuildOrderList(CasiaBot::ProductionQueue & queue)
{
	// 当前帧数（累计）
	int currentFrameCount = BWAPI::Broodwar->getFrameCount();

	// 判断是否需要增加母巢
	if (base_count + base_in_queue + base_being_built <= 4 && currentFrameCount > 10 && currentFrameCount % 200 == 0)
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

		if (base_count + base_in_queue + base_being_built <= 1)
		{
			mineralDequePositive = IsDequeAllPositive(mineralNetIncrease);
			if (mineralDequePositive)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hatchery));
			}
		}
		else
		{
			mineralDequePositive = IsDequeAllPositive(mineralNetIncrease);
			gasDequePositive = IsDequeAllPositive(gasNetIncrease);
			if (mineralDequePositive && gasDequePositive)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hatchery));
			}
		}

		lastFrameCount = currentFrameCount;
		lastFrameMineralAmount = currentFrameMineralAmount;
		lastFrameGasAmount = currentFrameGasAmount;
	}

	// 判断前提建筑是否存在
	bool isSpawningPoolExist = spawning_pool_being_built + spawning_pool_count + spawning_pool_in_queue > 0;
	if (!isSpawningPoolExist)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spawning_Pool), true);
	}

	bool isExtractorExist = extractor_being_built + extractor_count + extractor_in_queue > 0;
	if (!isExtractorExist && drone_count >= 7 && spawning_pool_count > 0)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Extractor));
	}

	bool isDefilerMoundExist = defiler_mound_being_built + defiler_mound_count + defiler_mound_in_queue > 0;
	bool isSpireExist =spire_being_built + spire_count + spire_in_queue > 0;
	bool isHiveExist = hive_being_built + hive_count + hive_in_queue > 0;
	bool isQueenNestExist = queens_nest_being_built + queens_nest_count + queens_nest_in_queue > 0;
	bool isLairExist = lair_being_built + lair_count + lair_in_queue > 0;

	if (isDefilerMoundExist)	// 若蝎子巢存在
	{
		if (currentFrameCount > 15000)
		{
			if (!isSpireExist)	// 若飞龙塔不存在
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spire));
			}
		}
	}
	else	// 若蝎子巢不存在
	{
		if (isHiveExist)	// 若蜂巢存在
		{
			if (currentFrameCount > 18000)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Defiler_Mound));
			}
		}
		else	// // 若蜂巢不存在
		{
			if (isQueenNestExist)	// 若皇后巢存在
			{
				if (currentFrameCount > 15000)
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hive));
				}
			}
			else	// 若皇后巢不存在
			{
				if (isLairExist)	// 若兽穴存在
				{
					if (currentFrameCount > 12000 && isSpireExist)
					{
						queue.add(MetaType(BWAPI::UnitTypes::Zerg_Queens_Nest));
					}
					if (!isSpireExist)
					{
						queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spire));
					}
				}
				else if (currentFrameCount > 4800)	// 若兽穴不存在
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Lair));
				}
			}
		}
	}

	if (spawning_pool_count > 0 && queue.upgradeCount(BWAPI::UpgradeTypes::Metabolic_Boost) == 0)
	{
		queue.add(MetaType(BWAPI::UpgradeTypes::Metabolic_Boost));
	}
	if (hive_count > 0 && queue.upgradeCount(BWAPI::UpgradeTypes::Adrenal_Glands) == 0)
	{
		queue.add(MetaType(BWAPI::UpgradeTypes::Adrenal_Glands));
	}

	int currentFlyerCarapaceLevel = BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Zerg_Flyer_Carapace);
	bool isFlyerCarapaceUpgrading = BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Zerg_Flyer_Carapace);
	if (!isFlyerCarapaceUpgrading && currentFlyerCarapaceLevel < 1 && queue.upgradeCount(BWAPI::UpgradeTypes::Zerg_Flyer_Carapace) == 0 && spire_count > 0)
	{
		queue.add(MetaType(BWAPI::UpgradeTypes::Zerg_Flyer_Carapace));
	}

	bool notEnoughDrone = false;
	if (base_count == 1)
	{
		if (drone_count < 15)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone));
		}
		notEnoughDrone = drone_count < 12;
	}
	else
	{
		if (drone_count < hatchery_count * 10)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone));
		}
		notEnoughDrone = drone_count < 8 * hatchery_count;
	}

	if (enemy_army_supply < escalationMark)
	{
		// 判断需要建造多少部队
		int need_mutalisk_count = (int)(enemy_goliath_count * 1.2 + 2) - mutalisk_count - mutalisk_in_queue;
		if (need_mutalisk_count <= 0 && mutalisk_count + mutalisk_in_queue < 6)
		{
			need_mutalisk_count = 1;
		}
		int need_defiler_count = 1;
		int need_zergling_count = (int)(enemyTerranBarrackUnitsAmount * 1.5) - zergling_count - zergling_in_queue;
		if (need_zergling_count <= 0 && zergling_count + zergling_in_queue < 12)
		{
			need_zergling_count = 2;
		}
		if (notEnoughDrone)
		{
			need_mutalisk_count = mutalisk_count + mutalisk_in_queue < 6 ? 1 : 0;
			need_defiler_count = 0;
			need_zergling_count = zergling_count + zergling_in_queue < 12 ? 2 : 0;
		}

		// 穿插建造Mutalisk、Defiler、Zergling
		do
		{
			if (need_zergling_count > 0)
			{
				// 2个Zergling
				if (spawning_pool_count > 0)
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Zergling));
				}
				need_zergling_count -= 2;
			}
			if (need_mutalisk_count > 0)
			{
				if (spire_count + greater_spire_count > 0)
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Mutalisk));
				}
				need_mutalisk_count--;
			}
			if (need_defiler_count > 0)
			{
				if (defiler_mound_count > 0)
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Defiler));
				}
				need_defiler_count--;
			}
			if (need_mutalisk_count <= 0 && need_defiler_count <= 0 && need_zergling_count <= 0)
			{
				break;
			}

		} while (true);

		int extractorUpperBound = std::min(base_count + base_being_built, 3);
		if (extractor_count + extractor_being_built + extractor_in_queue < extractorUpperBound)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Extractor));
		}
	}
	else
	{
		bool isUltraliskCavernExist = ultralisk_cavern_being_built + ultralisk_cavern_count + ultralisk_cavern_in_queue > 0;
		if (!isUltraliskCavernExist)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Ultralisk_Cavern));
		}

		// 判断需要建造多少部队
		int need_mutalisk_count = (int)(enemy_goliath_count * 1.2 + 2) - mutalisk_count - mutalisk_in_queue;
		int need_defiler_count = 1;
		int need_zergling_count = (int)(enemyTerranBarrackUnitsAmount * 1.5) - zergling_count - zergling_in_queue;

		if (need_mutalisk_count <= 0 && mutalisk_count + mutalisk_in_queue < 6)
		{
			need_mutalisk_count = 1;
		}

		if (need_zergling_count <= 0 && zergling_count + zergling_in_queue < 12)
		{
			need_zergling_count = 2;
		}
		if (notEnoughDrone)
		{
			need_mutalisk_count = mutalisk_count + mutalisk_in_queue < 6 ? 1 : 0;
			need_defiler_count = 0;
			need_zergling_count = zergling_count + zergling_in_queue < 12 ? 2 : 0;
		}

		// 穿插建造Mutalisk、Defiler、Zergling、Ultralisk
		do
		{
			if (need_zergling_count > 0)
			{
				// 2个Zergling
				if (spawning_pool_count > 0)
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Zergling));
				}
				need_zergling_count -= 2;
			}
			if (need_mutalisk_count > 0)
			{
				if (spire_count > 0)
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Mutalisk));
				}
				need_mutalisk_count--;
			}
			if (need_defiler_count > 0)
			{
				if (defiler_mound_count > 0)
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Defiler));
				}
				need_defiler_count--;
			}
			if (ultralisk_cavern_count > 0)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Ultralisk));
			}	
			if (need_mutalisk_count <= 0 && need_defiler_count <= 0 && need_zergling_count <= 0)
			{
				break;
			}

		} while (true);

		int extractorUpperBound = std::min(base_count + base_being_built, 3);
		if (extractor_count + extractor_being_built + extractor_in_queue < extractorUpperBound)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Extractor));
		}
	}
}

void ActionZVTFactoriesUnits::updateCurrentState(ProductionQueue &queue)
{
	ActionZergBase::updateCurrentState(queue);

	enemyTerranBarrackUnitsAmount = enemy_marine_count + enemy_firebat_count + enemy_ghost_count + enemy_medic_count;
	enemyTerranFactoryUnitsAmount = enemy_vulture_count * 2 + enemy_tank_count * 2 + enemy_goliath_count * 2;
	enemyTerranMechanizationRate = enemyTerranBarrackUnitsAmount == 0 ? 10 : (double)enemyTerranFactoryUnitsAmount / (double)enemyTerranBarrackUnitsAmount;
	if (enemyTerranFactoryUnitsAmount == 0) enemyTerranMechanizationRate = 0;
}
