#include "Common.h"
#include "UnitData.h"

using namespace UAlbertaBot;

UnitData::UnitData() 
	: mineralsLost(0)
	, gasLost(0)
{
	int maxTypeID(0);
	for (const BWAPI::UnitType & t : BWAPI::UnitTypes::allUnitTypes())
	{
		maxTypeID = maxTypeID > t.getID() ? maxTypeID : t.getID();
	}

	numDeadUnits	    = std::vector<int>(maxTypeID + 1, 0);
	numUnits		    = std::vector<int>(maxTypeID + 1, 0);
	numProductingUnits	= std::vector<int>(maxTypeID + 1, 0);
}

void UnitData::updateUnit(BWAPI::Unit unit)
{
	if (!unit) { return; }

	bool firstSeen = false;
    auto & it = unitMap.find(unit);
	if (it == unitMap.end())
    {
        firstSeen = true;
        unitMap[unit] = UnitInfo();
    }
	// unit changed, maybe morph
	else
	{
		auto lastType = unitMap[unit].type;
		if (lastType != unit->getType())
		{
			numUnits[lastType.getID()]--;
			// morphing complete
			if (lastType == BWAPI::UnitTypes::Zerg_Egg)
			{
				numProductingUnits[lastType.getID()] -= 1;
				numUnits[unit->getType().getID()] += 1;
			}
			// morphing lurker complete
			if (lastType == BWAPI::UnitTypes::Zerg_Lurker_Egg)
			{
				numProductingUnits[BWAPI::UnitTypes::Zerg_Lurker.getID()] -= 1;
				numUnits[unit->getType().getID()] += 1;
			}

			// start morphing
			if (unit->getType() == BWAPI::UnitTypes::Zerg_Egg)
			{
				numProductingUnits[unit->getBuildType().getID()] += 1;
			}
			// start morphing lurker
			if (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker_Egg)
			{
				numProductingUnits[BWAPI::UnitTypes::Zerg_Lurker.getID()] += 1;
			}

			// constructing building
			if (unit->getType().isBuilding())
			{
				numProductingUnits[unit->getType().getID()] += 1;
			}
		}
		// when building complete
		if (unit->getType().isBuilding() && !unitMap[unit].completed && unit->isCompleted())
		{
			numProductingUnits[unit->getType().getID()] -= 1;
			numUnits[unit->getType().getID()] += 1;
		}
	}
    
	UnitInfo & ui   = unitMap[unit];
    ui.unit         = unit;
    ui.player       = unit->getPlayer();
	ui.lastPosition = unit->getPosition();
	ui.lastHealth   = unit->getHitPoints();
    ui.lastShields  = unit->getShields();
	ui.unitID       = unit->getID();
	ui.type         = unit->getType();
    ui.completed    = unit->isCompleted();

    if (firstSeen)
    {
        numUnits[unit->getType().getID()]++;
    }
}

void UnitData::removeUnit(BWAPI::Unit unit)
{
	if (!unit) { return; }

	mineralsLost += unit->getType().mineralPrice();
	gasLost += unit->getType().gasPrice();
	numUnits[unit->getType().getID()]--;
	numDeadUnits[unit->getType().getID()]++;
		
	unitMap.erase(unit);
}

void UnitData::removeBadUnits()
{
	for (auto iter(unitMap.begin()); iter != unitMap.end();)
	{
		if (badUnitInfo(iter->second))
		{
			numUnits[iter->second.type.getID()]--;
			iter = unitMap.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

const bool UnitData::badUnitInfo(const UnitInfo & ui) const
{
    if (!ui.unit)
    {
        return false;
    }

	// Cull away any refineries/assimilators/extractors that were destroyed and reverted to vespene geysers
	if (ui.unit->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser)
	{ 
		return true;
	}

	// If the unit is a building and we can currently see its position and it is not there
	if (ui.type.isBuilding() && BWAPI::Broodwar->isVisible(ui.lastPosition.x/32, ui.lastPosition.y/32) && !ui.unit->isVisible())
	{
		return true;
	}

	return false;
}

int UnitData::getGasLost() const 
{ 
    return gasLost; 
}

int UnitData::getMineralsLost() const 
{ 
    return mineralsLost; 
}

int UnitData::getNumUnits(BWAPI::UnitType t) const 
{ 
	return numUnits[t.getID()] + numProductingUnits[t.getID()];
}

int UnitData::getNumCompletedUnits(BWAPI::UnitType t) const
{
	return numUnits[t.getID()];
}

int UnitData::getNumDeadUnits(BWAPI::UnitType t) const 
{ 
    return numDeadUnits[t.getID()]; 
}

const std::map<BWAPI::Unit,UnitInfo> & UnitData::getUnits() const 
{ 
    return unitMap; 
}