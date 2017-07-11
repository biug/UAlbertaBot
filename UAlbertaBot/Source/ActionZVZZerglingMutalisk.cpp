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
	return false;
}

bool ActionZVZZerglingMutalisk::tick()
{
	return false;
}

void ActionZVZZerglingMutalisk::getBuildOrderList(UAlbertaBot::ProductionQueue & queue)
{
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
}
