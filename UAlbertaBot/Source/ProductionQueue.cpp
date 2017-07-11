#include "ProductionQueue.h"

using namespace UAlbertaBot;

ProductionQueue::ProductionQueue()
{
	clear();

	int maxTypeID(0);
	for (const BWAPI::UnitType & t : BWAPI::UnitTypes::allUnitTypes())
	{
		maxTypeID = maxTypeID > t.getID() ? maxTypeID : t.getID();
	}
	_unitCount = std::vector<int>(maxTypeID + 1, 0);

	maxTypeID = 0;
	for (const BWAPI::TechType & t : BWAPI::TechTypes::allTechTypes())
	{
		maxTypeID = maxTypeID > t.getID() ? maxTypeID : t.getID();
	}
	_techCount = std::vector<int>(maxTypeID + 1, 0);

	maxTypeID = 0;
	for (const BWAPI::UpgradeType & t : BWAPI::UpgradeTypes::allUpgradeTypes())
	{
		maxTypeID = maxTypeID > t.getID() ? maxTypeID : t.getID();
	}
	_upgradeCount = std::vector<int>(maxTypeID + 1, 0);
}

void ProductionQueue::add(const ProductionItem & item, bool priority)
{
	const MetaType & unit = item._unit;
	if (unit.isUnit())
	{
		_unitCount[unit.getUnitType().getID()] += 1;
	}
	else if (unit.isTech())
	{
		_techCount[unit.getTechType().getID()] += 1;
	}
	else if (unit.isUpgrade())
	{
		_upgradeCount[unit.getUpgradeType().getID()] += 1;
	}
	if (priority || unit.getUnitType() == BWAPI::UnitTypes::Zerg_Overlord)
	{
		_priorityQueue.push_front(item);
	}
	else if (unit.isBuilding() || unit.isTech() || unit.isUpgrade())
	{
		_buildingQueue.push_back(item);
	}
	else
	{
		_armyQueue.push_back(item);
	}
}

void ProductionQueue::clear()
{
	_buildingQueue.clear();
	_armyQueue.clear();
	_priorityQueue.clear();
	_readyQueue.clear();
	for (auto && count : _unitCount)
	{
		count = 0;
	}
}

int ProductionQueue::unitCount(BWAPI::UnitType type)
{
	return _unitCount[type.getID()];
}

int ProductionQueue::techCount(BWAPI::TechType type)
{
	return _techCount[type.getID()];
}

int ProductionQueue::upgradeCount(BWAPI::UpgradeType type)
{
	return _upgradeCount[type.getID()];
}

void ProductionQueue::launchReady()
{
	if (!_priorityQueue.empty())
	{
		_readyQueue.push_back(_priorityQueue.front());
		_priorityQueue.pop_front();
	}
	if (!_armyQueue.empty())
	{
		_readyQueue.push_back(_armyQueue.front());
		_armyQueue.pop_front();
	}
	if (!_buildingQueue.empty())
	{
		_readyQueue.push_back(_buildingQueue.front());
		_buildingQueue.pop_front();
	}
}

ProductionItem ProductionQueue::popReady()
{
	ProductionItem item = _readyQueue.front();
	_readyQueue.pop_front();
	MetaType unit = item._unit;
	if (unit.isUnit())
	{
		_unitCount[unit.getUnitType().getID()] -= 1;
	}
	else if (unit.isTech())
	{
		_techCount[unit.getTechType().getID()] -= 1;
	}
	else if (unit.isUpgrade())
	{
		_upgradeCount[unit.getUpgradeType().getID()] -= 1;
	}
	return item;
}

void ProductionQueue::retreat(const ProductionItem & item)
{
	const MetaType & unit = item._unit;
	if (unit.isUnit())
	{
		_unitCount[unit.getUnitType().getID()] += 1;
	}
	else if (unit.isTech())
	{
		_techCount[unit.getTechType().getID()] += 1;
	}
	else if (unit.isUpgrade())
	{
		_upgradeCount[unit.getUpgradeType().getID()] += 1;
	}
	if (unit.getUnitType() == BWAPI::UnitTypes::Zerg_Overlord)
	{
		_priorityQueue.push_back(item);
	}
	else if (unit.isBuilding() || unit.isTech() || unit.isUpgrade())
	{
		_buildingQueue.push_back(item);
	}
	else
	{
		_armyQueue.push_back(item);
	}
}

bool ProductionQueue::empty()
{
	return _buildingQueue.empty() && _armyQueue.empty() && _priorityQueue.empty() && _readyQueue.empty();
}