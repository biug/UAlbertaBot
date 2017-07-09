#include "ProductionQueue.h"

using namespace UAlbertaBot;

ProductionQueue::ProductionQueue()
{
	clear();
}

void ProductionQueue::add(MetaType meta, bool priority)
{
	if (meta.getRace() != BWAPI::Races::Zerg) return;
	if (priority)
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