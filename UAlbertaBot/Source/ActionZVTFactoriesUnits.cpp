#include "ActionZVTFactoriesUnits.h"
#include "BuildingManager.h"
#include "ActionHelper.h"
//#include "UnitUtil.h"
#include "InformationManager.h"

using namespace CasiaBot;
using namespace UAlbertaBot;
using namespace CasiaBot::ActionHelper;
//using namespace UAlbertaBot::UnitUtil;
using namespace std;

ActionZVTFactoriesUnits::ActionZVTFactoriesUnits()
{
	init();
}

void ActionZVTFactoriesUnits::init()
{
	if (!isInitialized)
	{
		updateCurrentState();
		mineralNetIncrease = { 0,0,0,0,0 };
		gasNetIncrease = { 0,0,0,0,0 };
		lastFrameCount = BWAPI::Broodwar->getFrameCount();
		lastFrameMineralAmount = BWAPI::Broodwar->self()->minerals();
		lastFrameGasAmount = BWAPI::Broodwar->self()->gas();
		isInitialized = true;
	}
}

bool ActionZVTFactoriesUnits::canDeployAction()
{
	updateState();
	if (enemyTerranMechanizationRate >= 1)
	{
		return true;
	}
	else
		return false;
}

bool ActionZVTFactoriesUnits::tick()
{
	updateState();
	if (enemyTerranMechanizationRate < 1)
	{
		return true;
	}
	else
		return false;
}

void ActionZVTFactoriesUnits::getBuildOrderList(UAlbertaBot::ProductionQueue & queue)
{
	updateState();

	// �ж��Ƿ���Ҫ����ĸ��
	int currentFrameCount = BWAPI::Broodwar->getFrameCount();
	if (hatch_count <= 4 && currentFrameCount && currentFrameCount % 200 == 0)
	{
		int currentFrameMineralAmount = BWAPI::Broodwar->self()->minerals();
		int currentFrameGasAmount = BWAPI::Broodwar->self()->gas();
		int diffMineralAmount = currentFrameMineralAmount - lastFrameMineralAmount;
		int diffGasAmount = currentFrameGasAmount - lastFrameGasAmount;

		mineralNetIncrease.pop_front();
		mineralNetIncrease.push_back(diffMineralAmount);
		gasNetIncrease.pop_front();
		gasNetIncrease.push_back(diffGasAmount);

		bool mineralDequePositive;
		bool gasDequePositive;

		if (hatch_count <= 2)
		{
			mineralDequePositive = IsDequeAllPositive(mineralNetIncrease);
			if (mineralDequePositive)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hatchery), true);
			}
		}
		else
		{
			mineralDequePositive = IsDequeAllPositive(mineralNetIncrease);
			gasDequePositive = IsDequeAllPositive(gasNetIncrease);
			if (mineralDequePositive && gasDequePositive)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hatchery), true);
			}
		}

		lastFrameCount = currentFrameCount;
		lastFrameMineralAmount = currentFrameMineralAmount;
		lastFrameGasAmount = currentFrameGasAmount;
	}

	// �ж�ǰ�Ὠ���Ƿ����
	bool isExtractorExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Extractor);
	if (!isExtractorExist)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Extractor), true);
	}

	bool isSpawningPoolExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Spawning_Pool);
	if (!isSpawningPoolExist)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spawning_Pool), true);
	}

	bool isDefilerMoundExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Defiler_Mound);
	bool isSpireExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Spire);
	bool isHiveExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Hive);
	bool isQueenNestExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Queens_Nest);
	bool isLairExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Lair);
	if (isDefilerMoundExist)	// ��Ы�ӳ�����
	{
		if (!isSpireExist)	// ��������������
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spire), true);
		}
	}
	else	// ��Ы�ӳ�������
	{
		if (isHiveExist)	// ���䳲����
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Defiler_Mound), true);
			if (!isSpireExist)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spire), true);
			}
		}
		else	// // ���䳲������
		{
			if (isQueenNestExist)	// ���ʺ󳲴���
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hive), true);
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Defiler_Mound), true);
				if (!isSpireExist)
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spire), true);
				}
			}
			else	// ���ʺ󳲲�����
			{
				if (isLairExist)	// ����Ѩ����
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Queens_Nest), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hive), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Defiler_Mound), true);
					if (!isSpireExist)
					{
						queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
						queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spire), true);
					}
				}
				else	// ����Ѩ������
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Lair), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Queens_Nest), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hive), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Defiler_Mound), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spire), true);
				}
			}
		}
	}

	queue.add(MetaType(BWAPI::UpgradeTypes::Metabolic_Boost), true);
	queue.add(MetaType(BWAPI::UpgradeTypes::Adrenal_Glands), true);

	int currentFlyerCarapaceLevel = BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Zerg_Flyer_Carapace);
	bool isFlyerCarapaceUpgrading = BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Zerg_Flyer_Carapace);
	if (!isFlyerCarapaceUpgrading && currentFlyerCarapaceLevel < 1)
	{
		queue.add(MetaType(BWAPI::UpgradeTypes::Zerg_Flyer_Carapace), true);
	}

	// �ж��˿��Ƿ�ӽ�����ϵͳ�ڲ���supply��ֵ��˫����
	if (BWAPI::Broodwar->self()->supplyUsed() + 8 >= BWAPI::Broodwar->self()->supplyTotal())
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Overlord), true);
	}

	if (enemy_army_supply < escalationMark)
	{
		// �ж���Ҫ������ٲ���
		int need_mutalisk_count = (int)(enemy_goliath_count * 1.2 + 2) - mutalisk_count;
		int need_defiler_count = 3;
		int need_zergling_count = enemyTerranFactoryUnitsAmount * 2 - zergling_count;

		// ���彨��Mutalisk��Defiler��Zergling
		do
		{
			if (need_zergling_count > 0)
			{
				// 2��Zergling
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Zergling));
				need_zergling_count -= 2;
			}
			if (need_mutalisk_count > 0)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Mutalisk));
				need_mutalisk_count--;
			}
			if (need_defiler_count > 0)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Defiler));
				need_defiler_count--;
			}
			if (need_mutalisk_count <= 0 && need_defiler_count <= 0 && need_zergling_count <= 0)
			{
				break;
			}

		} while (true);

		int extractorUpperBound = std::min(hatch_count, 3);
		int currentExtractorCount = (int)InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Extractor, BWAPI::Broodwar->self());
		if (currentExtractorCount < extractorUpperBound)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Extractor));
		}
	}
	else
	{
		bool isUltraliskCavernExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Ultralisk_Cavern);
		if (!isUltraliskCavernExist)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Ultralisk_Cavern), true);
		}

		// �ж���Ҫ������ٲ���
		int need_mutalisk_count = (int)(enemy_goliath_count * 1.2 + 2) - mutalisk_count;
		int need_defiler_count = 3;
		int need_zergling_count = enemyTerranFactoryUnitsAmount * 2 - zergling_count;

		// ���彨��Mutalisk��Defiler��Zergling��Ultralisk
		do
		{
			if (need_zergling_count > 0)
			{
				// 2��Zergling
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Zergling));
				need_zergling_count -= 2;
			}
			if (need_mutalisk_count > 0)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Mutalisk));
				need_mutalisk_count--;
			}
			if (need_defiler_count > 0)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Defiler));
				need_defiler_count--;
			}
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Ultralisk));
			if (need_mutalisk_count <= 0 && need_defiler_count <= 0 && need_zergling_count <= 0)
			{
				break;
			}

		} while (true);

		int extractorUpperBound = std::min(hatch_count, 3);
		int currentExtractorCount = (int)InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Extractor, BWAPI::Broodwar->self());
		if (currentExtractorCount < extractorUpperBound)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Extractor), true);
		}
	}
}

void ActionZVTFactoriesUnits::updateState()
{
	updateCurrentState();

	auto &info = InformationManager::Instance();
	auto self = BWAPI::Broodwar->self();
	auto enemy = BWAPI::Broodwar->enemy();
	hatch_count = info.getNumUnits(BWAPI::UnitTypes::Zerg_Hatchery, self) + info.getNumUnits(BWAPI::UnitTypes::Zerg_Lair, self) + info.getNumUnits(BWAPI::UnitTypes::Zerg_Hive, self);
	zergling_count = info.getNumUnits(BWAPI::UnitTypes::Zerg_Zergling, self);
	mutalisk_count = info.getNumUnits(BWAPI::UnitTypes::Zerg_Mutalisk, self);
	enemy_marine_count = info.getNumUnits(BWAPI::UnitTypes::Terran_Marine, enemy);
	enemy_firebat_count = info.getNumUnits(BWAPI::UnitTypes::Terran_Firebat, enemy);
	enemy_ghost_count = info.getNumUnits(BWAPI::UnitTypes::Terran_Ghost, enemy);
	enemy_medic_count = info.getNumUnits(BWAPI::UnitTypes::Terran_Medic, enemy);
	enemy_vulture_count = info.getNumUnits(BWAPI::UnitTypes::Terran_Vulture, enemy);
	enemy_tank_count = info.getNumUnits(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode, enemy);
	enemy_goliath_count = info.getNumUnits(BWAPI::UnitTypes::Terran_Goliath, enemy);

	enemyTerranBarrackUnitsAmount = enemy_marine_count + enemy_firebat_count + enemy_ghost_count + enemy_medic_count;
	enemyTerranFactoryUnitsAmount = enemy_vulture_count + enemy_tank_count + enemy_goliath_count;
	enemyTerranMechanizationRate = enemyTerranFactoryUnitsAmount / enemyTerranBarrackUnitsAmount;
}
