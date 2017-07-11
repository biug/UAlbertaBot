#pragma once

#include "ActionZergBase.h"
#include <deque>

namespace CasiaBot
{
	class ActionZVPZealot : public ActionZergBase
	{
	public:
		ActionZVPZealot();
		~ActionZVPZealot() {}
		void init();
		bool canDeployAction();
		bool tick();
		void getBuildOrderList(UAlbertaBot::ProductionQueue &queue);

	private:
		bool isInitialized = false;
		double enemyDragoonOverZealotRate;
		int lastFrameCount = 0;
		int lastFrameMineralAmount;
		int lastFrameGasAmount;
		std::deque<int> mineralNetIncrease;
		std::deque<int> gasNetIncrease;

		void updateState();
	};
}