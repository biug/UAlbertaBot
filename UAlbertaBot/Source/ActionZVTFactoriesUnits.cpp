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
	if (enemyTerranMechanizationRate >= 0.8)
	{
		return true;
	}
	else
		return false;
}

bool ActionZVTFactoriesUnits::tick()
{
	if (enemyTerranMechanizationRate < 0.8)
	{
		return true;
	}
	else
		return false;
}

void ActionZVTFactoriesUnits::getBuildOrderList(UAlbertaBot::ProductionQueue & queue)
{

	// �ж��Ƿ���Ҫ����ĸ��
	int drone_count_exist = InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Drone, BWAPI::Broodwar->self());
	int currentFrameCount = BWAPI::Broodwar->getFrameCount();
	if (hatch_count <= 4 && currentFrameCount > 10 && currentFrameCount % 200 == 0)
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
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hatchery), true);
			}
		}
		else
		{
			mineralDequePositive = IsDequeAllPositive(mineralNetIncrease);
			gasDequePositive = IsDequeAllPositive(gasNetIncrease);
			if (mineralDequePositive && gasDequePositive)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hatchery), true);
			}
		}

		lastFrameCount = currentFrameCount;
		lastFrameMineralAmount = currentFrameMineralAmount;
		lastFrameGasAmount = currentFrameGasAmount;
	}

	// �ж�ǰ�Ὠ���Ƿ����
	bool isExtractorExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Extractor) || extractor_count > 0;
	// extractor after spawning pool
	if (!isExtractorExist && drone_count_exist >= 7 && InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Spawning_Pool, BWAPI::Broodwar->self()) > 0)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Extractor), true);
	}

	bool isSpawningPoolExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Spawning_Pool) || spawning_pool_count > 0;
	if (!isSpawningPoolExist)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spawning_Pool), true);
	}

	bool isDefilerMoundExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Defiler_Mound) || defiler_mount_count > 0;
	bool isSpireExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Spire) || spire_count > 0;
	bool isHiveExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Hive) || hive_count > 0;
	bool isQueenNestExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Queens_Nest) || queens_nest_count > 0;
	bool isLairExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Lair) || lair_count > 0;
	if (isDefilerMoundExist)	// ��Ы�ӳ�����
	{
		if (currentFrameCount > 15000)
		{
			if (!isSpireExist)	// ��������������
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spire));
			}
		}
	}
	else	// ��Ы�ӳ�������
	{
		if (isHiveExist)	// ���䳲����
		{
			if (currentFrameCount > 18000)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Defiler_Mound));
			}
		}
		else	// // ���䳲������
		{
			if (isQueenNestExist)	// ���ʺ󳲴���
			{
				if (currentFrameCount > 15000)
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hive));
				}
			}
			else	// ���ʺ󳲲�����
			{
				if (isLairExist)	// ����Ѩ����
				{
					if (currentFrameCount > 12000 && isSpireExist)
					{
						queue.add(MetaType(BWAPI::UnitTypes::Zerg_Queens_Nest));
					}
					if (!isSpireExist)
					{
						queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spire));
					}
				}
				else if (currentFrameCount > 4800)	// ����Ѩ������
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Lair));
				}
			}
		}
	}

	if (InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Spawning_Pool, BWAPI::Broodwar->self()) > 0 && queue.upgradeCount(BWAPI::UpgradeTypes::Metabolic_Boost) == 0)
	{
		queue.add(MetaType(BWAPI::UpgradeTypes::Metabolic_Boost));
	}
	if (InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Hive, BWAPI::Broodwar->self()) > 0 && queue.upgradeCount(BWAPI::UpgradeTypes::Adrenal_Glands) == 0)
	{
		queue.add(MetaType(BWAPI::UpgradeTypes::Adrenal_Glands));
	}

	int currentFlyerCarapaceLevel = BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Zerg_Flyer_Carapace);
	bool isFlyerCarapaceUpgrading = BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Zerg_Flyer_Carapace);
	if (!isFlyerCarapaceUpgrading && currentFlyerCarapaceLevel < 1 && queue.upgradeCount(BWAPI::UpgradeTypes::Zerg_Flyer_Carapace) == 0 && spire_count > 0)
	{
		queue.add(MetaType(BWAPI::UpgradeTypes::Zerg_Flyer_Carapace));
	}

	bool notEnoughDrone = false;
	if (hatch_count == 1)
	{
		if (drone_count < 15)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
		}
		notEnoughDrone = drone_count < 12;
	}
	else
	{
		if (drone_count < hatch_count * 10)
		{
			queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
		}
		notEnoughDrone = drone_count < 8 * hatch_count;
	}

	if (enemy_army_supply < escalationMark)
	{
		// �ж���Ҫ������ٲ���
		int need_mutalisk_count = (int)(enemy_goliath_count * 1.2 + 2) - mutalisk_count;
		if (need_mutalisk_count <= 0 && mutalisk_count < 6)
		{
			need_mutalisk_count = 1;
		}
		int need_defiler_count = 1;
		int need_zergling_count = (int)(enemyTerranBarrackUnitsAmount * 1.5) - zergling_count;
		if (need_zergling_count <= 0 && zergling_count < 12)
		{
			need_zergling_count = 2;
		}
		if (notEnoughDrone)
		{
			need_mutalisk_count = mutalisk_count < 6 ? 1 : 0;
			need_defiler_count = 0;
			need_zergling_count = zergling_count < 12 ? 2 : 0;
		}

		// ���彨��Mutalisk��Defiler��Zergling
		do
		{
			if (need_zergling_count > 0)
			{
				// 2��Zergling
				if (InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Spawning_Pool, BWAPI::Broodwar->self()) > 0)
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Zergling));
				}
				need_zergling_count -= 2;
			}
			if (need_mutalisk_count > 0)
			{
				if (InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Spire, BWAPI::Broodwar->self()) > 0)
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Mutalisk));
				}
				need_mutalisk_count--;
			}
			if (need_defiler_count > 0)
			{
				if (InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Defiler_Mound, BWAPI::Broodwar->self()) > 0)
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Defiler));
				}
				need_defiler_count--;
			}
			if (need_mutalisk_count <= 0 && need_defiler_count <= 0 && need_zergling_count <= 0)
			{
				break;
			}

		} while (true);

		//int extractorUpperBound = std::min(hatch_count, 3);
		//int currentExtractorCount = (int)InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Extractor, BWAPI::Broodwar->self());
		//if (currentExtractorCount < extractorUpperBound)
		//{
		//	queue.add(MetaType(BWAPI::UnitTypes::Zerg_Extractor));
		//}
	}
	else
	{
		bool isUltraliskCavernExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Ultralisk_Cavern);
		if (!isUltraliskCavernExist)
		{
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

void ActionZVTFactoriesUnits::updateCurrentState(ProductionQueue &queue)
{
	ActionZergBase::updateCurrentState(queue);

	enemyTerranBarrackUnitsAmount = enemy_marine_count + enemy_firebat_count + enemy_ghost_count + enemy_medic_count;
	enemyTerranFactoryUnitsAmount = enemy_vulture_count * 2 + enemy_tank_count * 2 + enemy_goliath_count * 2;
	enemyTerranMechanizationRate = enemyTerranBarrackUnitsAmount == 0 ? 10 : (double)enemyTerranFactoryUnitsAmount / (double)enemyTerranBarrackUnitsAmount;
	if (enemyTerranFactoryUnitsAmount == 0) enemyTerranMechanizationRate = 0;
}
