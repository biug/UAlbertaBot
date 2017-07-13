#pragma once

#include <deque>
#include "ProductionItem.h"

namespace UAlbertaBot
{

	class ProductionQueue
	{
		enum ProductionTypeID {BUILDING, ARMY, WORKER, TECH, TYPE_MAX};
		int						_buildID;
		// unit
		std::deque<ProductionItem>	_buildingQueue;
		std::deque<ProductionItem>	_armyQueue;
		std::deque<ProductionItem>	_workerQueue;
		// tech & upgrade
		std::deque<ProductionItem>	_techUpgradeQueue;
		// priority queue
		std::deque<ProductionItem>	_priorityQueue;

		std::vector<int>	_unitCount;
		std::vector<int>	_techCount;
		std::vector<int>	_upgradeCount;
		std::vector<int>	_straightCount;
		BWAPI::Unit			_lastUnit;
		int					_straightUnitCount;
		int					_straightBuildingCount;
		int					_straightArmyCount;
		int					_straightWorkerCount;

	public:
		ProductionQueue();

		void checkSupply();
		void add(const ProductionItem & item, bool priority = false);
		void retreat(const ProductionItem & item, bool priority = false);

		int unitCount(BWAPI::UnitType type);
		int techCount(BWAPI::TechType type);
		int upgradeCount(BWAPI::UpgradeType type);
		void clear();

		ProductionItem popItem();

		bool empty();
	};
}