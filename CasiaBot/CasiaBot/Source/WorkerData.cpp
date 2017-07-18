#include "WorkerData.h"
#include "Micro.h"

using namespace CasiaBot;

WorkerData::WorkerData() 
{
     for (auto & unit : BWAPI::Broodwar->getAllUnits())
	{
		if ((unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field))
		{
			mineralPatchWorkersCountMap[unit] = 0;
		}
	}
}

void WorkerData::checkResources()
{
	for (auto & worker : workers)
	{
		CAB_ASSERT(worker && worker->exists(), "worker has been killed");
		if (!worker->isGatheringGas() && workerJobMap[worker] == WorkerJob::Gas)
		{
			setWorkerIdle(worker);
		}
		if (!worker->isGatheringMinerals() && workerJobMap[worker] == WorkerJob::Minerals)
		{
			setWorkerIdle(worker);
		}
	}
}

void WorkerData::workerDestroyed(BWAPI::Unit unit)
{
	if (!unit) { return; }

	clearPreviousJob(unit);
	workers.erase(unit);
}

void WorkerData::addWorker(BWAPI::Unit unit)
{
	if (!unit) { return; }

	if (workers.find(unit) == workers.end())
	{
		workers.insert(unit);
		workerJobMap[unit] = Default;
	}
}

void WorkerData::addGatheringWorker(BWAPI::Unit worker, WorkerJob job)
{
	if (!worker) { return; }

	assert(workers.find(worker) == workers.end());

	workers.insert(worker);
	if (job == Minerals)
	{
		setWorkerGatheringMineral(worker);
	}
	if (job == Gas)
	{
		setWorkerGatheringGas(worker);
	}
}

void WorkerData::addBuildingWorker(BWAPI::Unit worker, BWAPI::UnitType buildingType)
{
	if (!worker) { return; }

	assert(workers.find(worker) == workers.end());
	workers.insert(worker);
	setWorkerBuilding(worker, buildingType);
}

void WorkerData::addMineralBase(BWAPI::Unit mineralBase)
{
	if (!mineralBase) { return; }

	if (mineralBases.find(mineralBase) == mineralBases.end())
	{
		mineralBases.insert(mineralBase);
		auto & patches = mineralBaseMineralPatchMap[mineralBase];
		for (auto & unit : BWAPI::Broodwar->getAllUnits())
		{
			if (unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field
				&& unit->getDistance(mineralBase) < mineralPatchRadius)
			{
				patches.insert(unit);
				mineralPatchMineralBaseMap[unit] = mineralBase;
				mineralPatchWorkersCountMap[unit] = 0;
			}
		}
	}
}

void WorkerData::addRefinery(BWAPI::Unit refinery)
{
	if (!refinery) { return; }

	if (refineries.find(refinery) == refineries.end())
	{
		refineries.insert(refinery);
		refineryWorkersCountMap[refinery] = 0;
	}
}

void WorkerData::removeMineralBase(BWAPI::Unit mineralBase)
{	
	if (!mineralBase) { return; }

	mineralBases.erase(mineralBase);

	// re-balance workers in here
	for (auto & worker : workers)
	{
		// if a worker was working at this depot
		auto it = workerMineralBaseMap.find(worker);
		if (it != workerMineralBaseMap.end() && it->second == mineralBase)
		{
			setWorkerIdle(worker);
		}
	}
	mineralBaseWorkersMap.erase(mineralBase);
	mineralBaseMineralPatchMap.erase(mineralBase);
}

void WorkerData::removeRefinery(BWAPI::Unit refinery)
{
	if (!refinery) { return; }

	refineries.erase(refinery);

	// re-balance workers in here
	for (auto & worker : workers)
	{
		// if a worker was working at this depot
		if (workerRefineryMap.find(worker) != workerRefineryMap.end()
			&& workerRefineryMap[worker] == refinery)
		{
			setWorkerIdle(worker);
		}
	}
	refineryWorkersCountMap.erase(refinery);
	refineryWorkersMap.erase(refinery);
}

void WorkerData::updateMineralPatchWorkersCount(BWAPI::Unit mineralPatch, int offset)
{
	auto it = mineralPatchWorkersCountMap.find(mineralPatch);
	if (it == mineralPatchWorkersCountMap.end())
    {
		mineralPatchWorkersCountMap[mineralPatch] = offset;
    }
    else
    {
		it->second += offset;
    }
}

void WorkerData::setWorkerIdle(BWAPI::Unit worker)
{
	if (!worker) { return; }

	clearPreviousJob(worker);
	workerJobMap[worker] = WorkerJob::Idle;
}

void WorkerData::setWorkerGatheringMineral(BWAPI::Unit worker)
{
	if (!worker) { return; }

	clearPreviousJob(worker);
	workerJobMap[worker] = WorkerJob::Minerals;

	// find best mineral patch
	auto mineral = getClosestMineral(worker);
	auto mineralPatch = mineral.first;
	auto mineralBase = mineral.second;
	if (!mineralPatch || !mineralBase)
	{
		workerJobMap[worker] = WorkerJob::Idle;
		return;
	}

	workerMineralPatchMap[worker] = mineralPatch;
	updateMineralPatchWorkersCount(mineralPatch, 1);

	// set the mineral the worker is working on
	workerMineralBaseMap[worker] = mineralBase;
	mineralBaseWorkersMap[mineralBase].insert(worker);

	// right click the mineral to start mining
	Micro::SmartRightClick(worker, mineralPatch);
}

void WorkerData::setWorkerGatheringGas(BWAPI::Unit worker)
{
	if (!worker) { return; }

	clearPreviousJob(worker);
	workerJobMap[worker] = WorkerJob::Gas;

	auto refinery = getClosestRefinery(worker);
	if (!refinery)
	{
		workerJobMap[worker] = WorkerJob::Idle;
		return;
	}

	// increase the count of workers assigned to this refinery
	refineryWorkersCountMap[refinery] += 1;

	// set the refinery the worker is working on
	workerRefineryMap[worker] = refinery;
	refineryWorkersMap[refinery].insert(worker);

	// right click the refinery to start harvesting
	Micro::SmartRightClick(worker, refinery);
}

void WorkerData::setWorkerBuilding(BWAPI::Unit worker, BWAPI::UnitType bulidingType)
{
	if (!worker) { return; }

	clearPreviousJob(worker);
	workerJobMap[worker] = WorkerJob::Build;

	workerBuildingTypeMap[worker] = bulidingType;
}

void WorkerData::setWorkerMoving(BWAPI::Unit worker, WorkerMoveData wmd)
{
	if (!worker) { return; }

	clearPreviousJob(worker);
	workerJobMap[worker] = WorkerJob::Move;

	workerMoveMap[worker] = wmd;
}

void WorkerData::setWorkerRepairing(BWAPI::Unit worker, BWAPI::Unit building)
{
	if (!worker) { return; }

	clearPreviousJob(worker);
	workerJobMap[worker] = WorkerJob::Repair;

	assert(worker->getType() == BWAPI::UnitTypes::Terran_SCV);

	// set the building the worker is to repair
	workerRepairMap[worker] = building;

	// start repairing 
	if (!worker->isRepairing())
	{
		Micro::SmartRepair(worker, building);
	}
}

void WorkerData::setWorkerScouting(BWAPI::Unit worker)
{
	if (!worker) { return; }

	clearPreviousJob(worker);
	workerJobMap[worker] = WorkerJob::Scout;
}

void WorkerData::setWorkerCombating(BWAPI::Unit worker)
{
	if (!worker) { return; }

	clearPreviousJob(worker);
	workerJobMap[worker] = WorkerJob::Combat;
}

void WorkerData::clearPreviousJob(BWAPI::Unit worker)
{
	if (!worker) { return; }

	WorkerJob previousJob = getWorkerJob(worker);

	if (previousJob == Minerals)
	{
		auto mineralBase = workerMineralBaseMap[worker];

		mineralBaseWorkersMap[mineralBase].erase(worker);
		workerMineralBaseMap.erase(worker);

        // remove a worker from this unit's assigned mineral patch
		updateMineralPatchWorkersCount(workerMineralPatchMap[worker], -1);

        // erase the association from the map
		workerMineralPatchMap.erase(worker);
	}
	else if (previousJob == Gas)
	{
		auto refinery = workerRefineryMap[worker];
		refineryWorkersCountMap[refinery] -= 1;

		refineryWorkersMap[refinery].erase(worker);
		workerRefineryMap.erase(worker);
	}
	else if (previousJob == Build)
	{
		workerBuildingTypeMap.erase(worker);
	}
	else if (previousJob == Repair)
	{
		workerRepairMap.erase(worker);
	}
	else if (previousJob == Move)
	{
		workerMoveMap.erase(worker);
	}

	workerJobMap.erase(worker);
}

int WorkerData::getNumWorkers() const
{
	return workers.size();
}

int WorkerData::getNumMineralWorkers() const
{
	size_t num = 0;
	for (auto & unit : workers)
	{
		if (workerJobMap.at(unit) == WorkerData::Minerals)
		{
			num++;
		}
	}
	return num;
}

int WorkerData::getNumGasWorkers() const
{
	size_t num = 0;
	for (auto & unit : workers)
	{
		if (workerJobMap.at(unit) == WorkerData::Gas)
		{
			num++;
		}
	}
	return num;
}

int	WorkerData::getNumMineralPatchWorkers(BWAPI::Unit mineralPatch) const
{
	auto it = mineralPatchWorkersCountMap.find(mineralPatch);
	if (it == mineralPatchWorkersCountMap.end())
	{
		return 0;
	}
	return it->second;
}

int WorkerData::getNumRefineryWorkers(BWAPI::Unit refinery) const
{
	auto it = refineryWorkersCountMap.find(refinery);
	if (it == refineryWorkersCountMap.end())
	{
		return 0;
	}
	return it->second;
}

int WorkerData::getNumIdleWorkers() const
{
	size_t num = 0;
	for (auto & unit : workers)
	{
		if (workerJobMap.at(unit) == WorkerData::Idle)
		{
			num++;
		}
	}
	return num;
}


enum WorkerData::WorkerJob WorkerData::getWorkerJob(BWAPI::Unit worker) const
{
	if (!worker) { return Default; }

	auto it = workerJobMap.find(worker);

	if (it != workerJobMap.end())
	{
		return it->second;
	}

	return Default;
}

bool WorkerData::isWorkerInOverloadMineralPatch(BWAPI::Unit worker) const
{
	if (!worker) { return false; }

	auto it = workerMineralPatchMap.find(worker);

	if (it == workerMineralPatchMap.end())
	{
		return true;
	}

	auto patch = it->second;

	auto it2 = mineralPatchWorkersCountMap.find(patch);

	if (it2 == mineralPatchWorkersCountMap.end())
	{
		return true;
	}

	return it2->second > Config::Macro::WorkersPerMineralPatch;
}

bool WorkerData::isMineralBase(BWAPI::Unit base) const
{
	for (auto & mineralBase : mineralBases)
	{
		if (base->getDistance(mineralBase) < sameMineralBaseRadius)
		{
			return false;
		}
	}
	return base->getType().isResourceDepot()
		&& base->isCompleted()
		&& base->getPlayer() == BWAPI::Broodwar->self();
}

bool WorkerData::isRefinery(BWAPI::Unit refinery) const
{
	return refinery->getType().isRefinery()
		&& refinery->isCompleted()
		&& refinery->getPlayer() == BWAPI::Broodwar->self();
}

std::pair<BWAPI::Unit, BWAPI::Unit> WorkerData::getClosestMineral(BWAPI::Unit worker) const
{
	if (!worker) { return{ nullptr, nullptr }; }

	// get the depot associated with this unit
	BWAPI::Unit bestPatch = nullptr;
	BWAPI::Unit bestMineralBase = nullptr;
	int			bestDist = 100000;

	for (auto mineralBase : mineralBases)
	{
		auto it = mineralBaseMineralPatchMap.find(mineralBase);
		if (it == mineralBaseMineralPatchMap.end()) { continue; }

		for (auto patch : it->second)
		{
			auto it2 = mineralPatchWorkersCountMap.find(patch);
			if (it2 == mineralPatchWorkersCountMap.end()) { continue; }

			if (it2->second < Config::Macro::WorkersPerMineralPatch)
			{
				int dist = worker->getDistance(patch);
				if (dist < bestDist)
				{
					bestDist = dist;
					bestPatch = patch;
					bestMineralBase = mineralBase;
				}
			}
		}
	}
	return{ bestPatch, bestMineralBase };
}

BWAPI::Unit WorkerData::getClosestRefinery(BWAPI::Unit worker) const
{
	if (!worker) { return nullptr; }

	// get the depot associated with this unit
	BWAPI::Unit bestRefinery = nullptr;
	int			bestDist = 100000;

	for (auto refinery : refineries)
	{
		auto it = refineryWorkersCountMap.find(refinery);
		if (it == refineryWorkersCountMap.end()) { continue; }
		if (it->second < Config::Macro::WorkersPerRefinery)
		{
			int dist = worker->getDistance(refinery);
			if (dist < bestDist)
			{
				bestDist = dist;
				bestRefinery = refinery;
			}
		}
	}
	return bestRefinery;
}

BWAPI::Unit WorkerData::getLarvaDepot() const
{
	for (auto mineralBase : mineralBases)
	{
		auto it = mineralBaseMineralPatchMap.find(mineralBase);
		if (it == mineralBaseMineralPatchMap.end()) { continue; }

		for (auto patch : it->second)
		{
			auto it2 = mineralPatchWorkersCountMap.find(patch);
			if (it2 == mineralPatchWorkersCountMap.end()) { continue; }

			if (it2->second < Config::Macro::WorkersPerMineralPatch)
			{
				return mineralBase;
			}
		}
	}
	return nullptr;
}

BWAPI::Unit WorkerData::getWorkerRepairUnit(BWAPI::Unit unit) const
{
	if (!unit) { return nullptr; }

	auto it = workerRepairMap.find(unit);

	if (it != workerRepairMap.end())
	{
		return it->second;
	}	

	return nullptr;
}

BWAPI::Unit WorkerData::getRefineryWorker(BWAPI::Unit refinery) const
{
	if (!refinery) { return nullptr; }

	auto it = refineryWorkersMap.find(refinery);

	if (it != refineryWorkersMap.end() && it->second.size() > 0)
	{
		return *it->second.begin();
	}

	return nullptr;
}

BWAPI::Unit WorkerData::getWorkerMineralBase(BWAPI::Unit worker) const
{
	if (!worker) { return nullptr; }

	auto it = workerMineralBaseMap.find(worker);

	if (it != workerMineralBaseMap.end())
	{
		return it->second;
	}	

	return nullptr;
}

BWAPI::Unit WorkerData::getWorkerMineralPatch(BWAPI::Unit worker) const
{
	if (!worker) { return nullptr; }

	auto it = workerMineralPatchMap.find(worker);

	if (it != workerMineralPatchMap.end())
	{
		return it->second;
	}

	return nullptr;
}

BWAPI::Unit WorkerData::getWorkerRefinery(BWAPI::Unit worker) const
{
	if (!worker) { return nullptr; }

	auto it = workerRefineryMap.find(worker);

	if (it != workerRefineryMap.end())
	{
		return it->second;
	}

	return nullptr;
}

BWAPI::UnitType	WorkerData::getWorkerBuildingType(BWAPI::Unit unit) const
{
	if (!unit) { return BWAPI::UnitTypes::None; }

	auto it = workerBuildingTypeMap.find(unit);

	if (it != workerBuildingTypeMap.end())
	{
		return it->second;
	}	

	return BWAPI::UnitTypes::None;
}

WorkerMoveData WorkerData::getWorkerMoveData(BWAPI::Unit unit) const
{
	auto it = workerMoveMap.find(unit);

	assert(it != workerMoveMap.end());
	
	return (it->second);
}

char WorkerData::getJobCode(BWAPI::Unit unit) const
{
	if (!unit) { return 'X'; }

	WorkerData::WorkerJob j = getWorkerJob(unit);

	if (j == WorkerData::Build) return 'B';
	if (j == WorkerData::Combat) return 'C';
	if (j == WorkerData::Default) return 'D';
	if (j == WorkerData::Gas) return 'G';
	if (j == WorkerData::Idle) return 'I';
	if (j == WorkerData::Minerals) return 'M';
	if (j == WorkerData::Repair) return 'R';
	if (j == WorkerData::Move) return 'O';
	if (j == WorkerData::Scout) return 'S';
	return 'X';
}

void WorkerData::drawMineralDebugInfo()
{
}