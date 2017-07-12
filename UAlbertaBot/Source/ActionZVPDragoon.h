#pragma once

#include "ActionZergBase.h"
#include <deque>

namespace CasiaBot
{
	class ActionZVPDragoon : public ActionZergBase
	{
	public:
		ActionZVPDragoon();
		~ActionZVPDragoon() {}
		void init() override;
		bool canDeployAction() override;
		bool tick() override;
		void getBuildOrderList(UAlbertaBot::ProductionQueue &queue) override;
		void updateCurrentState(UAlbertaBot::ProductionQueue &queue) override;

	private:
		double enemyDragoonOverZealotRate = 0;
	};
}