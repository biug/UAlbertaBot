#include "ProductionQueue.h"
#include "InformationManager.h"

using namespace UAlbertaBot;

ProductionQueue::ProductionQueue()
{
	clear();
}

void ProductionQueue::add(const ProductionItem & item, bool priority)
{
	const MetaType & unit = item._unit;

	if (unit.isUnit()) _unitCount[unit.getUnitType().getID()] += 1;
	else if (unit.isTech()) _techCount[unit.getTechType().getID()] += 1;
	else if (unit.isUpgrade()) _upgradeCount[unit.getUpgradeType().getID()] += 1;

	if (priority)
	{
		_priorityQueue.push_back(item);
	}
	else if (unit.getUnitType() == BWAPI::UnitTypes::Zerg_Overlord)
	{
		_priorityQueue.push_back(item);
	}
	else if (unit.isBuilding())
	{
		_buildingQueue.push_back(item);
	}
	else if (unit.getUnitType().isWorker())
	{
		_workerQueue.push_back(item);
	}
	else if (unit.isUnit())
	{
		_armyQueue.push_back(item);
	}
	else
	{
		_techUpgradeQueue.push_back(item);
	}
}

void ProductionQueue::checkSupply()
{
	int supply = BWAPI::Broodwar->self()->supplyTotal() / 2;
	int supplyUsed = (BWAPI::Broodwar->self()->supplyUsed() + 1) / 2;
	int overlordInQueue = unitCount(BWAPI::UnitTypes::Zerg_Overlord.getID());
	int overlordInConstructing =
		InformationManager::Instance().getNumConstructingUnits(BWAPI::UnitTypes::Zerg_Overlord, BWAPI::Broodwar->self());
	int overlordReady = overlordInQueue + overlordInConstructing;
	if (supply - supplyUsed <= 6)
	{
		if (supply <= 9)
		{
			if (supply - supplyUsed <= 0 && overlordReady == 0)
			{
				add(MetaType(BWAPI::UnitTypes::Zerg_Overlord));
			}
		}
		else if (supply <= 36)
		{
			if (supply - supplyUsed <= 3 && overlordReady == 0)
			{
				add(MetaType(BWAPI::UnitTypes::Zerg_Overlord));
			}
		}
		else
		{
			if (overlordReady <= 1)
			{
				add(MetaType(BWAPI::UnitTypes::Zerg_Overlord));
			}
		}
	}
}

void ProductionQueue::retreat(const ProductionItem & item, bool priority)
{
	const MetaType & unit = item._unit;

	if (unit.isUnit()) _unitCount[unit.getUnitType().getID()] += 1;
	else if (unit.isTech()) _techCount[unit.getTechType().getID()] += 1;
	else if (unit.isUpgrade()) _upgradeCount[unit.getUpgradeType().getID()] += 1;

	if (priority)
	{
		_priorityQueue.push_front(item);
	}
	else if (unit.getUnitType() == BWAPI::UnitTypes::Zerg_Overlord)
	{
		_priorityQueue.push_front(item);
	}
	else if (unit.isBuilding())
	{
		_buildingQueue.push_front(item);
	}
	else if (unit.getUnitType().isWorker())
	{
		_workerQueue.push_front(item);
	}
	else if (unit.isUnit())
	{
		_armyQueue.push_front(item);
	}
	else
	{
		_techUpgradeQueue.push_front(item);
	}
}

void ProductionQueue::clear()
{
	_buildID = -1;
	_buildingQueue.clear();
	_armyQueue.clear();
	_workerQueue.clear();
	_techUpgradeQueue.clear();
	_priorityQueue.clear();
	_lastUnit = nullptr;
	_straightUnitCount = 0;

	// unit count vector
	int maxTypeID(0);
	for (const BWAPI::UnitType & t : BWAPI::UnitTypes::allUnitTypes())
	{
		maxTypeID = maxTypeID > t.getID() ? maxTypeID : t.getID();
	}
	_unitCount = std::vector<int>(maxTypeID + 1, 0);
	_straightCount = std::vector<int>(maxTypeID + 1, 0);

	// tech count vector
	maxTypeID = 0;
	for (const BWAPI::TechType & t : BWAPI::TechTypes::allTechTypes())
	{
		maxTypeID = maxTypeID > t.getID() ? maxTypeID : t.getID();
	}
	_techCount = std::vector<int>(maxTypeID + 1, 0);

	// upgrade count vector
	maxTypeID = 0;
	for (const BWAPI::UpgradeType & t : BWAPI::UpgradeTypes::allUpgradeTypes())
	{
		maxTypeID = maxTypeID > t.getID() ? maxTypeID : t.getID();
	}
	_upgradeCount = std::vector<int>(maxTypeID + 1, 0);
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

ProductionItem ProductionQueue::popItem()
{
	if (!_priorityQueue.empty())
	{
		ProductionItem item = _priorityQueue.front();
		_priorityQueue.pop_front();
		return item;
	}
	_buildID += 1;
	MetaType meta;
	ProductionItem retItem(meta);
	if (_buildID == ProductionTypeID::TYPE_MAX) _buildID = 0;
	switch (_buildID)
	{
	case ProductionTypeID::BUILDING:
		if (!_buildingQueue.empty())
		{
			retItem = _buildingQueue.front();
			_buildingQueue.pop_front();
		}
		break;
	case ProductionTypeID::ARMY:
		if(!_armyQueue.empty())
		{
			retItem = _armyQueue.front();
			_armyQueue.pop_front();
		}
		break;
	case ProductionTypeID::WORKER:
		if (!_workerQueue.empty())
		{
			retItem = _workerQueue.front();
			_workerQueue.pop_front();
		}
		break;
	case ProductionTypeID::TECH:
		if (!_techUpgradeQueue.empty())
		{
			retItem = _techUpgradeQueue.front();
			_techUpgradeQueue.pop_front();
		}
		break;
	}
	MetaType unit = retItem._unit;
	if (unit.isUnit()) _unitCount[unit.getUnitType().getID()] -= 1;
	else if (unit.isTech()) _techCount[unit.getTechType().getID()] -= 1;
	else if (unit.isUpgrade()) _upgradeCount[unit.getUpgradeType().getID()] -= 1;
	return retItem;
}

bool ProductionQueue::empty()
{
	return _buildingQueue.empty()
		&& _armyQueue.empty()
		&& _priorityQueue.empty()
		&& _workerQueue.empty()
		&& _techUpgradeQueue.empty();
}