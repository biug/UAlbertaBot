#pragma once

#include <deque>
#include "MetaType.h"

namespace UAlbertaBot
{

	class ProductionQueue
	{
		ProductionQueue();

		std::deque<MetaType>	_buildingQueue;
		std::deque<MetaType>	_armyQueue;
		std::deque<MetaType>	_priorityQueue;

	public:
		std::deque<MetaType>	_readyQueue;

		void add(MetaType item, bool priority = false);
		void clear();

		void launchReady();
		bool checkReady();
	};
}