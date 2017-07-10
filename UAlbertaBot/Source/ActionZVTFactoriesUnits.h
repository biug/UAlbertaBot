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
		const double escalationMark = 80;	// �з��˿ڴﵽ��ֵʱ��ʼ������

		void updateState();
	};
}