#include "ActionZVZZerglingMutalisk.h"
#include "BuildingManager.h"
#include "ActionHelper.h"
#include "InformationManager.h"

using namespace CasiaBot;
using namespace UAlbertaBot;
using namespace CasiaBot::ActionHelper;
using namespace std;

ActionZVZZerglingMutalisk::ActionZVZZerglingMutalisk()
{
	init();
}

void ActionZVZZerglingMutalisk::init()
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

bool ActionZVZZerglingMutalisk::canDeployAction()
{
	if (enemy_mutalisk_count > 0 || (enemy_hydralisk_count == 0 && enemy_mutalisk_count == 0))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ActionZVZZerglingMutalisk::tick()
{
	if (enemy_hydralisk_count > 0 && enemy_mutalisk_count == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ActionZVZZerglingMutalisk::getBuildOrderList(UAlbertaBot::ProductionQueue & queue)
{
	// ���й�������
	int drone_count_exist = InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Drone, BWAPI::Broodwar->self());
	// ��ǰ֡�����ۼƣ�
	int currentFrameCount = BWAPI::Broodwar->getFrameCount();

	// �ж��Ƿ���Ҫ����ĸ��
	if (hatchery_count <= 4 && currentFrameCount > 10 && currentFrameCount % 200 == 0)
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

		if (hatchery_count <= 2)
		{
			mineralDequePositive = IsDequeAllPositive(mineralNetIncrease);
			if (mineralDequePositive)
			{
				//queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hatchery), true);
			}
		}
		else
		{
			mineralDequePositive = IsDequeAllPositive(mineralNetIncrease);
			gasDequePositive = IsDequeAllPositive(gasNetIncrease);
			if (mineralDequePositive && gasDequePositive)
			{
				//queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone), true);
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hatchery), true);
			}
		}

		lastFrameCount = currentFrameCount;
		lastFrameMineralAmount = currentFrameMineralAmount;
		lastFrameGasAmount = currentFrameGasAmount;
	}

	// �ж�ǰ�Ὠ���Ƿ����
	bool isExtractorExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Extractor) || extractor_count > 0;
	if (!isExtractorExist && drone_count_exist >= 7 && InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Spawning_Pool, BWAPI::Broodwar->self()))
	{
		//queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone));
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Extractor), true);
	}

	bool isSpawningPoolExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Spawning_Pool) || spawning_pool_count > 0;
	if (!isSpawningPoolExist)
	{
		//queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone));
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spawning_Pool), true);
	}

	bool isHiveExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Hive) || hive_count > 0;
	bool isQueenNestExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Queens_Nest) || queens_nest_count > 0;
	bool isLairExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Lair) || lair_count > 0;
	if (!isHiveExist)	// ���䳲������
	{
		if (isQueenNestExist)	// ���ʺ󳲴���
		{
			if (currentFrameCount > 10800)
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Hive));
			}
		}
		else	// ���ʺ󳲲�����
		{
			if (isLairExist)	// ����Ѩ����
			{
				if (currentFrameCount > 9000)
				{
					queue.add(MetaType(BWAPI::UnitTypes::Zerg_Queens_Nest));
				}
			}
			else if (currentFrameCount > 4800)	// ����Ѩ������
			{
				queue.add(MetaType(BWAPI::UnitTypes::Zerg_Lair));
			}
		}
	}

	bool isSpireExist = BuildingManager::Instance().isBeingBuilt(BWAPI::UnitTypes::Zerg_Spire) || spire_count > 0;
	if (!isSpireExist && currentFrameCount > 6000)	// ��������������
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Spire));
	}

	if (InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Spawning_Pool, BWAPI::Broodwar->self()) > 0 && queue.upgradeCount(BWAPI::UpgradeTypes::Metabolic_Boost) == 0)
	{
		queue.add(MetaType(BWAPI::UpgradeTypes::Metabolic_Boost));
	}
	if (InformationManager::Instance().getNumUnits(BWAPI::UnitTypes::Zerg_Hive, BWAPI::Broodwar->self()) > 0 && queue.upgradeCount(BWAPI::UpgradeTypes::Adrenal_Glands) == 0)
	{
		queue.add(MetaType(BWAPI::UpgradeTypes::Adrenal_Glands));
	}

	// �ж���Ҫ������ٲ���
	int need_zergling_count = (int)(enemy_zergling_count * 1.5) - zergling_count;
	if (need_zergling_count <= 0 && zergling_count < 24)
	{
		need_zergling_count = 2;
	}
	int need_mutalisk_count = std::min(1, need_zergling_count / 3);

	// ���彨��Zergling��Mutalisk
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
		if (need_zergling_count <= 0 && need_mutalisk_count <= 0)
		{
			break;
		}

	} while (true);

	if (drone_count < hatchery_count * 15)
	{
		queue.add(MetaType(BWAPI::UnitTypes::Zerg_Drone));
	}
}

void ActionZVZZerglingMutalisk::updateCurrentState(ProductionQueue &queue)
{
	ActionZergBase::updateCurrentState(queue);
}
