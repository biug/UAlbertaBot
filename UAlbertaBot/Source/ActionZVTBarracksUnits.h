#pragma once

#include "ActionZergBase.h"
#include "ProductionQueue.h"
#include <deque>

namespace CasiaBot
{
	class ActionZVTBarracksUnits : public ActionZergBase
	{
	public:
		void init();
		bool canDeployAction();
		bool tick();
		void getBuildOrderList(UAlbertaBot::ProductionQueue &queue);
	private:
		int enermyTerranBarracksUnitsAmount;
		int enermyTerranFactoryUnitsAmount;
		double enermyTerranMechanizationRate;
		int lastFrameCount = 0;
		int lastFrameMineralAmount;
		int lastFrameGasAmount;
		std::deque<int> mineralNetIncrease;
		std::deque<int> gasNetIncrease;

		void updateState();
		bool isDequeAllPositive(std::deque<int> &queue);
	};
}