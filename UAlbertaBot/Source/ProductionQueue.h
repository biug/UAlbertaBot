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

	public:
		std::deque<ProductionItem>	_readyQueue;

		ProductionQueue();

		void add(const ProductionItem & item, bool priority = false);
		void clear();

		void launchReady();
		bool checkReady();

		void retreat(const ProductionItem & item);

		bool empty();
		int overlordCount();
	};
}