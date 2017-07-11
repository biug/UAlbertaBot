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
		void init() override;
		bool canDeployAction() override;
		bool tick() override;
		void getBuildOrderList(UAlbertaBot::ProductionQueue &queue) override;
		void updateCurrentState(UAlbertaBot::ProductionQueue &queue) override;

	private:
		bool isInitialized = false;
		double enemyDragoonOverZealotRate;
		int lastFrameCount = 0;
		int lastFrameMineralAmount;
		int lastFrameGasAmount;
		std::deque<int> mineralNetIncrease;
		std::deque<int> gasNetIncrease;
	};
}