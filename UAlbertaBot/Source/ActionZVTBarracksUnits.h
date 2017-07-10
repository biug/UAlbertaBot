#pragma once

#include "ActionZergBase.h"
#include <deque>

namespace CasiaBot
{
	class ActionZVTBarracksUnits : public ActionZergBase
	{
	public:
		ActionZVTBarracksUnits();
		~ActionZVTBarracksUnits(){}
		void init();
		bool canDeployAction();
		bool tick();
		void getBuildOrderList(UAlbertaBot::ProductionQueue &queue);
	private:
		bool isInitialized = false;
		int enermyTerranBarrackUnitsAmount;
		int enermyTerranFactoryUnitsAmount;
		double enermyTerranMechanizationRate;
		int lastFrameCount = 0;
		int lastFrameMineralAmount;
		int lastFrameGasAmount;
		std::deque<int> mineralNetIncrease;
		std::deque<int> gasNetIncrease;

		void updateState();
	};
}