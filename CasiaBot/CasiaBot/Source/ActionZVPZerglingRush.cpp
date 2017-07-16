#include "ActionZVPZerglingRush.h"
#include "BuildingManager.h"
#include "ActionHelper.h"
#include "InformationManager.h"

using namespace CasiaBot;
using namespace CasiaBot::ActionHelper;
using namespace std;

ActionZVPZerglingRush::ActionZVPZerglingRush()
{
}

void ActionZVPZerglingRush::init()
{
	if (!isInitialized)
	{
		lastFrameCount = BWAPI::Broodwar->getFrameCount();
		lastFrameMineralAmount = BWAPI::Broodwar->self()->minerals();
		lastFrameGasAmount = BWAPI::Broodwar->self()->gas();
		isInitialized = true;
	}
}

bool ActionZVPZerglingRush::canDeployAction()
{
	return true;
}

bool ActionZVPZerglingRush::tick()
{
	return false;
}

void ActionZVPZerglingRush::getBuildOrderList(ProductionQueue & queue)
{
	// 当前帧数（累计）
	int currentFrameCount = BWAPI::Broodwar->getFrameCount();

	if (!isFirstDroneInQueue)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spawning_Pool), true);
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Zergling), true);
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Zergling), true);
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Zergling), true);
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Zergling), true);
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
		isFirstDroneInQueue = true;
	}
	else
	{
		if (zergling_count + zergling_in_queue > 6 && zergling_count + zergling_in_queue <= 10)
		{
			// 没小狗或小狗少的时候，农民补到5个
			if (drone_count + drone_in_queue < 5)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone));
			}
		}
		else if (zergling_count + zergling_in_queue > 10 && zergling_count + zergling_in_queue <= 20)
		{
			// 农民补到6个
			if (drone_count + drone_in_queue < 6)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone));
			}
		}
		else if (zergling_count + zergling_in_queue > 20)
		{
			// 农民补到7个
			if (drone_count + drone_in_queue < 7)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone));
			}
		}


		// 判断前提建筑是否存在
		bool isSpawningPoolExist = spawning_pool_being_built + spawning_pool_count + spawning_pool_in_queue > 0;
		if (currentFrameCount > 2400 && !isSpawningPoolExist)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spawning_Pool), true);
		}

		// 判断需要建造多少部队
		const int total_zergling_count = 180;
		int need_zergling_count = total_zergling_count - zergling_count - zergling_in_queue;
		if (spawning_pool_count > 0 && need_zergling_count > 0)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Zergling));
		}

		int minerals = BWAPI::Broodwar->self()->minerals();
		if (minerals > 400 && base_count + base_in_queue + base_being_built <= 2)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hatchery));
		}
		else if (minerals > 800 && base_count + base_in_queue + base_being_built <= 3)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hatchery));
		}

		if (base_count >= 2 && zergling_count >= 20 && drone_count + drone_in_queue < 8)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone));
		}
	}
}

void ActionZVPZerglingRush::updateCurrentState(ProductionQueue & queue)
{
	ActionZergBase::updateCurrentState(queue);
}
