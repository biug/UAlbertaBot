#include "ActionZVZZerglingMutalisk.h"
#include "BuildingManager.h"
#include "ActionHelper.h"
#include "InformationManager.h"

using namespace CasiaBot;
using namespace CasiaBot;
using namespace CasiaBot::ActionHelper;
using namespace std;

ActionZVZZerglingMutalisk::ActionZVZZerglingMutalisk()
{
}

void ActionZVZZerglingMutalisk::init()
{
	if (!isInitialized)
	{
		lastFrameCount = BWAPI::Broodwar->getFrameCount();
		lastFrameMineralAmount = BWAPI::Broodwar->self()->minerals();
		lastFrameGasAmount = BWAPI::Broodwar->self()->gas();
		isInitialized = true;
	}
}

bool ActionZVZZerglingMutalisk::canDeployAction()
{
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
	if (enemy_hydralisk_count > 0 && enemy_mutalisk_count == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ActionZVZZerglingMutalisk::getBuildOrderList(CasiaBot::ProductionQueue & queue)
{
	// 当前帧数（累计）
	int currentFrameCount = BWAPI::Broodwar->getFrameCount();
	int currentFrameMineralAmount = BWAPI::Broodwar->self()->minerals();
	int currentFrameGasAmount = BWAPI::Broodwar->self()->gas();
	int diffMineralAmount = currentFrameMineralAmount - lastFrameMineralAmount;
	int diffGasAmount = currentFrameGasAmount - lastFrameGasAmount;

	mineralNetIncrease.pop_front();
	mineralNetIncrease.push_back(diffMineralAmount);
	gasNetIncrease.pop_front();
	gasNetIncrease.push_back(diffGasAmount);

	bool mineralDequePositive = IsDequeAllPositive(mineralNetIncrease);
	bool gasDequePositive = IsDequeNoneNegative(gasNetIncrease);;

	// 判断是否需要增加母巢
	if (base_count + base_in_queue + base_being_built <= 4 && currentFrameCount > 10 && currentFrameCount % 200 == 0)
	{
		

		if (base_count + base_in_queue + base_being_built <= 2)
		{
			if (mineralDequePositive)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hatchery));
			}
		}
		else
		{
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

	bool isHiveExist = hive_being_built + hive_count + hive_in_queue > 0;
	bool isQueenNestExist = queens_nest_being_built + queens_nest_count + queens_nest_in_queue > 0;
	bool isLairExist = lair_being_built + lair_count + lair_in_queue > 0;
	if (!isHiveExist)	// 若蜂巢不存在
	{
		if (isQueenNestExist)	// 若皇后巢存在
		{
			if (currentFrameCount > 10800)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hive));
			}
		}
		else	// 若皇后巢不存在
		{
			if (isLairExist)	// 若兽穴存在
			{
				if (currentFrameCount > 9000)
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Queens_Nest));
				}
			}
			else if (currentFrameCount > 4800)	// 若兽穴不存在
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Lair));
			}
		}
	}

	bool isSpireExist = spire_count + spire_being_built + spire_in_queue > 0;
	if (!isSpireExist && currentFrameCount > 6000)	// 若飞龙塔不存在
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spire));
	}

	if (spawning_pool_count > 0 && queue.upgradeCount(BWAPI::UpgradeTypes::Metabolic_Boost) == 0)
	{
		queue.add(MetaType(BWAPI::UpgradeTypes::Metabolic_Boost));
	}
	if (hive_count > 0 && queue.upgradeCount(BWAPI::UpgradeTypes::Adrenal_Glands) == 0)
	{
		queue.add(MetaType(BWAPI::UpgradeTypes::Adrenal_Glands));
	}

	bool notEnoughDrone = false;
	if (base_count == 1)
	{
		if (drone_count + drone_in_queue < 15)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone));
		}
		notEnoughDrone = drone_count + drone_in_queue < 12;
	}
	else
	{
		if (drone_count + drone_in_queue < hatchery_count * 10)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone));
		}
		notEnoughDrone = drone_count + drone_in_queue < 8 * hatchery_count;
	}

	// 判断需要建造多少部队
	int need_zergling_count = 0;
	if (isSpawningPoolExist)
	{
		//首先根据敌方单位数量判断
		need_zergling_count = std::max(need_zergling_count, (int)(enemy_zergling_count * 1.5) - zergling_count - zergling_in_queue);
		//在资源富余的情况下继续生产
		if (mineralDequePositive && gasDequePositive  && need_zergling_count < 2 && zergling_in_queue < 6)
			need_zergling_count = 2;
		//优先补农民
		if (notEnoughDrone && zergling_count + zergling_in_queue >= 12)
		{
			need_zergling_count = 0;
		}
	}
	int need_mutalisk_count = 0;
	if (isSpireExist)
	{
		//首先根据敌方单位数量判断
		need_mutalisk_count = std::max(0, need_zergling_count / 3);
		//在资源富余的情况下继续生产
		if (mineralDequePositive && gasDequePositive  && need_mutalisk_count < 1 && mutalisk_in_queue < 3)
			need_mutalisk_count = 1;
		//优先补农民
		if (notEnoughDrone && mutalisk_count + mutalisk_in_queue >= 3)
		{
			need_mutalisk_count = 0;
		}
	}

	

	// 穿插建造Zergling和Mutalisk
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
		if (need_zergling_count <= 0 && need_mutalisk_count <= 0)
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

void ActionZVZZerglingMutalisk::updateCurrentState(ProductionQueue &queue)
{
	ActionZergBase::updateCurrentState(queue);
}
