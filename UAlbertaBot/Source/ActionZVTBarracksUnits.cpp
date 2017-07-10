#include "ActionZVTBarracksUnits.h"
#include "BuildingManager.h"
#include <vector>

using namespace CasiaBot;
using namespace UAlbertaBot;
using namespace std;

void ActionZVTBarracksUnits::init()
{
	updateCurrentState();
	mineralNetIncrease = { 0,0,0,0,0 };
	gasNetIncrease = { 0,0,0,0,0 };
	lastFrameCount = BWAPI::Broodwar->getFrameCount();
	lastFrameMineralAmount = BWAPI::Broodwar->self()->minerals();
	lastFrameGasAmount = BWAPI::Broodwar->self()->gas();
}

bool ActionZVTBarracksUnits::canDeployAction()
{
	updateState();
	if (enermyTerranMechanizationRate <= 0.5)
	{
		return true;
	}
	else
		return false;
}

bool ActionZVTBarracksUnits::tick()
{
	updateState();
	if (enermyTerranMechanizationRate >= 1)
	{
		return true;
	}
	else
		return false;
}

void ActionZVTBarracksUnits::getBuildOrderList(UAlbertaBot::ProductionQueue & queue)
{
	// �ж��Ƿ���Ҫ����ĸ��
	int currentFrameCount = BWAPI::Broodwar->getFrameCount();
	if (currentFrameCount && currentFrameCount % 200 == 0)
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
			mineralDequePositive = isDequeAllPositive(mineralNetIncrease);
			if (mineralDequePositive)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hatchery), true);
			}
		}
		else
		{
			mineralDequePositive = isDequeAllPositive(mineralNetIncrease);
			gasDequePositive = isDequeAllPositive(gasNetIncrease);
			if (mineralDequePositive && gasDequePositive)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hatchery), true);
			}
		}

		lastFrameCount = currentFrameCount;
		lastFrameMineralAmount = currentFrameMineralAmount;
		lastFrameGasAmount = currentFrameGasAmount;
	}

	// �ж�ǰ�Ὠ���Ƿ����
	bool isExtractorExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Extractor);
	if (!isExtractorExist)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Extractor), true);
	}

	bool isSpawningPoolExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Spawning_Pool);
	if (!isSpawningPoolExist)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spawning_Pool), true);
	}

	bool isHydraliskDenExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Hydralisk_Den);
	if (!isHydraliskDenExist)
	{
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

	// �ж��˿��Ƿ�ӽ�����ϵͳ�ڲ���supply��ֵ��˫����
	if (BWAPI::Broodwar->self()->supplyUsed() + 8 >= BWAPI::Broodwar->self()->supplyTotal())
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Overlord), true);
	}

	// �ж���Ҫ������ٲ���
	int need_zergling_count = (int)(enermyTerranBarracksUnitsAmount * 1.5) - zergling_count;
	int need_lurker_count = need_zergling_count / 2;

	// ���彨��Zergling��Lurker
	do
	{
		if (need_zergling_count > 0)
		{
			// 2��Zergling
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

	

}

void ActionZVTBarracksUnits::updateState()
{
	updateCurrentState();
	enermyTerranBarracksUnitsAmount = enermy_marine_count + enermy_firebat_count + enermy_ghost_count + enermy_medic_count;
	enermyTerranFactoryUnitsAmount = enermy_vulture_count + enermy_tank_count + enermy_goliath_count;
	enermyTerranMechanizationRate = enermyTerranFactoryUnitsAmount / enermyTerranBarracksUnitsAmount;
}

bool ActionZVTBarracksUnits::isDequeAllPositive(std::deque<int>& queue)
{
	for (size_t i = 0; i < queue.size(); i++)
	{
		if (queue[i] <= 0)
		{
			return false;
		}
	}
	return true;
}
