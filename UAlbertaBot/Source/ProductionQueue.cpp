#include "ProductionQueue.h"

using namespace UAlbertaBot;

ProductionQueue::ProductionQueue()
{
	clear();
}

void ProductionQueue::add(MetaType meta, bool priority)
{
	if (priority || meta.getUnitType() == BWAPI::UnitTypes::Zerg_Overlord)
	{
		_priorityQueue.push_back(meta);
	}
	else if (meta.isBuilding() || meta.isTech() || meta.isUpgrade())
	{
		_buildingQueue.push_back(meta);
	}
	else
	{
		_armyQueue.push_back(meta);
	}
}

void ProductionQueue::clear()
{
	_buildingQueue.clear();
	_armyQueue.clear();
	_priorityQueue.clear();
	_readyQueue.clear();
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

bool ProductionQueue::checkReady()
{
	return !_readyQueue.empty();
}

void ProductionQueue::retreat(MetaType & unit)
{
	if (unit.getUnitType() == BWAPI::UnitTypes::Zerg_Overlord)
	{
		_priorityQueue.push_front(unit);
	}
	else if (unit.isBuilding() || unit.isTech() || unit.isUpgrade())
	{
		_buildingQueue.push_front(unit);
	}
	else
	{
		_armyQueue.push_front(unit);
	}
}

bool ProductionQueue::empty()
{
	return _buildingQueue.empty() && _armyQueue.empty() && _priorityQueue.empty();
}

int ProductionQueue::overlordCount()
{
	int count = 0;
	for (int i = 0; i < _priorityQueue.size(); ++i)
	{
		if (_priorityQueue[i].getUnitType() == BWAPI::UnitTypes::Zerg_Overlord)
		{
			++count;
		}
	}
	return count;
}