#pragma once

#include <deque>
#include "ProductionItem.h"

namespace UAlbertaBot
{

	class ProductionQueue
	{

		std::deque<ProductionItem>	_buildingQueue;
		std::deque<ProductionItem>	_armyQueue;
		std::deque<ProductionItem>	_priorityQueue;
		std::vector<int>			_unitCount;
		std::vector<int>			_techCount;
		std::vector<int>			_upgradeCount;

	public:
		std::deque<ProductionItem>	_readyQueue;

		ProductionQueue();

		void add(const ProductionItem & item, bool priority = false);
		int unitCount(BWAPI::UnitType type);
		int techCount(BWAPI::TechType type);
		int upgradeCount(BWAPI::UpgradeType type);
		void clear();

		void launchReady();
		ProductionItem popReady();

		void retreat(const ProductionItem & item);

		bool empty();
	};
}