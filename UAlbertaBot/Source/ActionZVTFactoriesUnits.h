#pragma once

#include "ActionZergBase.h"
#include <deque>

namespace CasiaBot
{
	class ActionZVTFactoriesUnits : public ActionZergBase
	{
	public:
		ActionZVTFactoriesUnits();
		~ActionZVTFactoriesUnits(){}
		void init() override;
		bool canDeployAction() override;
		bool tick() override;
		void getBuildOrderList(UAlbertaBot::ProductionQueue &queue) override;
		void updateCurrentState(UAlbertaBot::ProductionQueue &queue) override;

	private:
		bool isInitialized = false;
		int enemyTerranBarrackUnitsAmount;
		int enemyTerranFactoryUnitsAmount;
		double enemyTerranMechanizationRate;
		int lastFrameCount = 0;
		int lastFrameMineralAmount;
		int lastFrameGasAmount;
		std::deque<int> mineralNetIncrease;
		std::deque<int> gasNetIncrease;
		const double escalationMark = 240;	// 敌方人口达到此值时开始造雷兽
	};
}