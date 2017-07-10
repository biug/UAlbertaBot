#include "ActionZVTFactoriesUnits.h"
#include "BuildingManager.h"
#include "ActionHelper.h"

using namespace CasiaBot;
using namespace UAlbertaBot;
using namespace CasiaBot::ActionHelper;
using namespace std;

void ActionZVTFactoriesUnits::init()
{
	updateCurrentState();
	mineralNetIncrease = { 0,0,0,0,0 };
	gasNetIncrease = { 0,0,0,0,0 };
	lastFrameCount = BWAPI::Broodwar->getFrameCount();
	lastFrameMineralAmount = BWAPI::Broodwar->self()->minerals();
	lastFrameGasAmount = BWAPI::Broodwar->self()->gas();
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
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hatchery), true);
			}
		}
		else
		{
			mineralDequePositive = IsDequeAllPositive(mineralNetIncrease);
			gasDequePositive = IsDequeAllPositive(gasNetIncrease);
			if (mineralDequePositive && gasDequePositive)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hatchery), true);
			}
		}

		lastFrameCount = currentFrameCount;
		lastFrameMineralAmount = currentFrameMineralAmount;
		lastFrameGasAmount = currentFrameGasAmount;
	}


}

void ActionZVTFactoriesUnits::updateState()
{
	updateCurrentState();
	enermyTerranBarrackUnitsAmount = enermy_marine_count + enermy_firebat_count + enermy_ghost_count + enermy_medic_count;
	enermyTerranFactoryUnitsAmount = enermy_vulture_count + enermy_tank_count + enermy_goliath_count;
	enermyTerranMechanizationRate = enermyTerranFactoryUnitsAmount / enermyTerranBarrackUnitsAmount;
}
