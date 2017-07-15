#include "WorkerData.h"
#include "Micro.h"

using namespace CasiaBot;

WorkerData::WorkerData() 
{
     for (auto & unit : BWAPI::Broodwar->getAllUnits())
	{
		if ((unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field))
		{
            workersOnMineralPatch[unit] = 0;
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
			setWorkerJob(worker, Idle, nullptr);
		}
		if (!worker->isGatheringMinerals() && workerJobMap[worker] == WorkerJob::Minerals)
		{
			setWorkerJob(worker, Idle, nullptr);
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

void WorkerData::addWorker(BWAPI::Unit unit, WorkerJob job, BWAPI::Unit jobUnit)
{
	if (!unit || !jobUnit) { return; }

	assert(workers.find(unit) == workers.end());

	workers.insert(unit);
	setWorkerJob(unit, job, jobUnit);
}

void WorkerData::addWorker(BWAPI::Unit unit, enum WorkerJob job, BWAPI::UnitType jobUnitType)
{
	if (!unit) { return; }

	assert(workers.find(unit) == workers.end());
	workers.insert(unit);
	setWorkerJob(unit, job, jobUnitType);
}

void WorkerData::addMineral(BWAPI::Unit unit)
{
	if (!unit) { return; }

	if (minerals.find(unit) == minerals.end())
	{
		minerals.insert(unit);
		mineralWorkerCount[unit] = 0;
	}
}

void WorkerData::addRefinery(BWAPI::Unit unit)
{
	if (!unit) { return; }

	if (refineries.find(unit) == refineries.end())
	{
		refineries.insert(unit);
		refineryWorkerCount[unit] = 0;
	}
}

void WorkerData::removeMineral(BWAPI::Unit unit)
{	
	if (!unit) { return; }

	minerals.erase(unit);

	// re-balance workers in here
	for (auto & worker : workers)
	{
		// if a worker was working at this depot
		if (workerMineralMap.find(worker) != workerMineralMap.end()
			&& workerMineralMap[worker] == unit)
		{
			setWorkerJob(worker, Idle, nullptr);
		}
	}
	mineralWorkerCount.erase(unit);
	workersInMineral.erase(unit);
}

void WorkerData::removeRefinery(BWAPI::Unit unit)
{
	if (!unit) { return; }

	refineries.erase(unit);

	// re-balance workers in here
	for (auto & worker : workers)
	{
		// if a worker was working at this depot
		if (workerRefineryMap.find(worker) != workerRefineryMap.end()
			&& workerRefineryMap[worker] == unit)
		{
			setWorkerJob(worker, Idle, nullptr);
		}
	}
	refineryWorkerCount.erase(unit);
	workersInRefinery.erase(unit);
}

void WorkerData::addToMineralPatch(BWAPI::Unit unit, int num)
{
    if (workersOnMineralPatch.find(unit) == workersOnMineralPatch.end())
    {
        workersOnMineralPatch[unit] = num;
    }
    else
    {
        workersOnMineralPatch[unit] += num;
    }
}

void WorkerData::setWorkerJob(BWAPI::Unit unit, enum WorkerJob job, BWAPI::Unit jobUnit)
{
	if (!unit) { return; }

	clearPreviousJob(unit);
	workerJobMap[unit] = job;

	if (job == Minerals)
	{

        BWAPI::Unit mineralToMine = getMineralToMine(unit);
        workerMineralAssignment[unit] = mineralToMine;
        addToMineralPatch(mineralToMine, 1);

		BWAPI::Unit depot = *minerals.begin();
		int minDis = 1000000;
		for (auto & base : minerals)
		{
			int dis = base->getDistance(mineralToMine);
			if (dis < minDis)
			{
				minDis = dis;
				depot = base;
			}
		}

		// increase the number of workers assigned to this nexus
		mineralWorkerCount[depot] += 1;

		// set the mineral the worker is working on
		workerMineralMap[unit] = depot;
		workersInMineral[depot].insert(unit);

		// right click the mineral to start mining
		Micro::SmartRightClick(unit, mineralToMine);
	}
	else if (job == Gas)
	{
		// increase the count of workers assigned to this refinery
		refineryWorkerCount[jobUnit] += 1;

		// set the refinery the worker is working on
		workerRefineryMap[unit] = jobUnit;
		workersInRefinery[jobUnit].insert(unit);

		// right click the refinery to start harvesting
		Micro::SmartRightClick(unit, jobUnit);
	}
	else if (job == Scout)
	{

	}
    else if (job == Build)
    {
        BWAPI::Broodwar->printf("Setting worker job to build");
    }
}

void WorkerData::setWorkerJob(BWAPI::Unit unit, enum WorkerJob job, BWAPI::UnitType jobUnitType)
{
	if (!unit) { return; }

	clearPreviousJob(unit);
	workerJobMap[unit] = job;

	if (job == Build)
	{
		workerBuildingTypeMap[unit] = jobUnitType;
	}
}

void WorkerData::setWorkerJob(BWAPI::Unit unit, enum WorkerJob job, WorkerMoveData wmd)
{
	if (!unit) { return; }

	clearPreviousJob(unit);
	workerJobMap[unit] = job;

	if (job == Move)
	{
		workerMoveMap[unit] = wmd;
	}

	if (workerJobMap[unit] != Move)
	{
		BWAPI::Broodwar->printf("Something went horribly wrong");
	}
}


void WorkerData::clearPreviousJob(BWAPI::Unit unit)
{
	if (!unit) { return; }

	WorkerJob previousJob = getWorkerJob(unit);

	if (previousJob == Minerals)
	{
		auto mineral = workerMineralMap[unit];
		mineralWorkerCount[mineral] -= 1;

		workersInMineral[mineral].erase(unit);
		workerMineralMap.erase(unit);

        // remove a worker from this unit's assigned mineral patch
        addToMineralPatch(workerMineralAssignment[unit], -1);

        // erase the association from the map
        workerMineralAssignment.erase(unit);
	}
	else if (previousJob == Gas)
	{
		auto refinery = workerRefineryMap[unit];
		refineryWorkerCount[refinery] -= 1;

		workersInRefinery[refinery].erase(unit);
		workerRefineryMap.erase(unit);
	}
	else if (previousJob == Build)
	{
		workerBuildingTypeMap.erase(unit);
	}
	else if (previousJob == Repair)
	{
		workerRepairMap.erase(unit);
	}
	else if (previousJob == Move)
	{
		workerMoveMap.erase(unit);
	}

	workerJobMap.erase(unit);
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


enum WorkerData::WorkerJob WorkerData::getWorkerJob(BWAPI::Unit unit)
{
	if (!unit) { return Default; }

	std::hash_map<BWAPI::Unit, enum WorkerJob>::iterator it = workerJobMap.find(unit);

	if (it != workerJobMap.end())
	{
		return it->second;
	}

	return Default;
}

bool WorkerData::mineralIsFull(BWAPI::Unit mineral)
{
	if (!mineral) { return false; }

	int assignedWorkers = getNumAssignedWorkers(mineral);
	int mineralsNearDepot = getMineralsNearDepot(mineral);

	if (assignedWorkers > mineralsNearDepot * 2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int WorkerData::numMineralNeedWorker(BWAPI::Unit mineral)
{
	if (!mineral) { return false; }

	int assignedWorkers = getNumAssignedWorkers(mineral);
	int mineralsNearDepot = getMineralsNearDepot(mineral);

	return mineralsNearDepot * 2 - assignedWorkers;
}

BWAPI::Unitset WorkerData::getMineralPatchesNearDepot(BWAPI::Unitset bases)
{
    // if there are minerals near the depot, add them to the set
    BWAPI::Unitset mineralsNearDepot;

    int radius = 300;

	for (auto & unit : BWAPI::Broodwar->getAllUnits())
	{
		for (auto & base : bases)
		{
			if ((unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field) && unit->getDistance(base) < radius)
			{
				mineralsNearDepot.insert(unit);
			}
		}
	}

    // if we didn't find any, use the whole map
    if (mineralsNearDepot.empty())
    {
        for (auto & unit : BWAPI::Broodwar->getAllUnits())
	    {
		    if ((unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field))
		    {
                mineralsNearDepot.insert(unit);
		    }
	    }
    }

    return mineralsNearDepot;
}

int WorkerData::getMineralsNearDepot(BWAPI::Unit depot)
{
	if (!depot) { return 0; }

	int mineralsNearDepot = 0;

	for (auto & unit : BWAPI::Broodwar->getAllUnits())
	{
		if ((unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field) && unit->getDistance(depot) < 300)
		{
			mineralsNearDepot++;
		}
	}

	return mineralsNearDepot;
}

BWAPI::Unit WorkerData::getWorkerResource(BWAPI::Unit unit)
{
	if (!unit) { return nullptr; }

	// create the iterator
	std::hash_map<BWAPI::Unit, BWAPI::Unit>::iterator it;
	
	// if the worker is mining, set the iterator to the mineral map
	if (getWorkerJob(unit) == Minerals)
	{
		it = workerMineralMap.find(unit);
		if (it != workerMineralMap.end())
		{
			return it->second;
		}	
	}
	else if (getWorkerJob(unit) == Gas)
	{
		it = workerRefineryMap.find(unit);
		if (it != workerRefineryMap.end())
		{
			return it->second;
		}	
	}

	return nullptr;
}


BWAPI::Unit WorkerData::getMineralToMine(BWAPI::Unit worker)
{
	if (!worker) { return nullptr; }


	// get the depot associated with this unit
	BWAPI::Unit bestMineral = nullptr;
	double bestDist = 100000;
    double bestNumAssigned = 10000;

	BWAPI::Unitset bases;
	for (auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		if ((unit->getType() == BWAPI::UnitTypes::Zerg_Hatchery && unit->isCompleted())
			|| unit->getType() == BWAPI::UnitTypes::Zerg_Lair
			|| unit->getType() == BWAPI::UnitTypes::Zerg_Hive)
		{
			bases.insert(unit);
		}
	}
    BWAPI::Unitset mineralPatches = getMineralPatchesNearDepot(bases);

	for (auto & mineral : mineralPatches)
	{
		double dist = mineral->getDistance(worker);
        double numAssigned = workersOnMineralPatch[mineral];

        if (numAssigned < bestNumAssigned)
        {
            bestMineral = mineral;
            bestDist = dist;
            bestNumAssigned = numAssigned;
        }
		else if (numAssigned == bestNumAssigned)
		{
			if (dist < bestDist)
            {
                bestMineral = mineral;
                bestDist = dist;
                bestNumAssigned = numAssigned;
            }
		}
	}

	return bestMineral;
}

BWAPI::Unit WorkerData::getLarvaDepot()
{
	int minNum = -100;
	BWAPI::Unit depot = nullptr;
	for (auto & mineral : minerals)
	{
		if (mineral->getType() == BWAPI::UnitTypes::Zerg_Hatchery && mineral->isConstructing())
		{
			continue;
		}
		int num = numMineralNeedWorker(mineral);
		if (num > minNum)
		{
			minNum = num;
			depot = mineral;
		}
	}
	return depot;
}

BWAPI::Unit WorkerData::getWorkerRepairUnit(BWAPI::Unit unit)
{
	if (!unit) { return nullptr; }

	std::hash_map<BWAPI::Unit, BWAPI::Unit>::iterator it = workerRepairMap.find(unit);

	if (it != workerRepairMap.end())
	{
		return it->second;
	}	

	return nullptr;
}

BWAPI::Unit WorkerData::getMineralWorker(BWAPI::Unit unit)
{
	if (!unit) { return nullptr; }

	std::hash_map<BWAPI::Unit, std::set<BWAPI::Unit>>::iterator it = workersInMineral.find(unit);

	if (it != workersInMineral.end() && it->second.size() > 0)
	{
		return *it->second.begin();
	}

	return nullptr;
}

BWAPI::Unit WorkerData::getRefineryWorker(BWAPI::Unit unit)
{
	if (!unit) { return nullptr; }

	std::hash_map<BWAPI::Unit, std::set<BWAPI::Unit>>::iterator it = workersInRefinery.find(unit);

	if (it != workersInRefinery.end() && it->second.size() > 0)
	{
		return *it->second.begin();
	}

	return nullptr;
}

BWAPI::Unit WorkerData::getWorkerMineral(BWAPI::Unit unit)
{
	if (!unit) { return nullptr; }

	std::hash_map<BWAPI::Unit, BWAPI::Unit>::iterator it = workerMineralMap.find(unit);

	if (it != workerMineralMap.end())
	{
		return it->second;
	}	

	return nullptr;
}

BWAPI::UnitType	WorkerData::getWorkerBuildingType(BWAPI::Unit unit)
{
	if (!unit) { return BWAPI::UnitTypes::None; }

	std::hash_map<BWAPI::Unit, BWAPI::UnitType>::iterator it = workerBuildingTypeMap.find(unit);

	if (it != workerBuildingTypeMap.end())
	{
		return it->second;
	}	

	return BWAPI::UnitTypes::None;
}

WorkerMoveData WorkerData::getWorkerMoveData(BWAPI::Unit unit)
{
	std::hash_map<BWAPI::Unit, WorkerMoveData>::iterator it = workerMoveMap.find(unit);

	assert(it != workerMoveMap.end());
	
	return (it->second);
}

bool WorkerData::isMineralPatchEnough()
{
	for (auto & mineral : minerals)
	{
		if (mineral->getType() == BWAPI::UnitTypes::Zerg_Hatchery && mineral->isConstructing())
		{
			continue;
		}
		if (!mineralIsFull(mineral))
		{
			return true;
		}
	}
	return false;
}

int WorkerData::getNumAssignedWorkers(BWAPI::Unit unit)
{
	if (!unit) { return 0; }

	std::hash_map<BWAPI::Unit, int>::iterator it;
	
	// if the worker is mining, set the iterator to the mineral map
	if (unit->getType().isResourceDepot())
	{
		it = mineralWorkerCount.find(unit);

		// if there is an entry, return it
		if (it != mineralWorkerCount.end())
		{
			return it->second;
		}
	}
	else if (unit->getType().isRefinery())
	{
		it = refineryWorkerCount.find(unit);

		// if there is an entry, return it
		if (it != refineryWorkerCount.end())
		{
			return it->second;
		}
		// otherwise, we are only calling this on completed refineries, so set it
		else
		{
			refineryWorkerCount[unit] = 0;
		}
	}

	// when all else fails, return 0
	return 0;
}

int WorkerData::getNumAssignedWorkersOnPatch(BWAPI::Unit unit)
{
	if (!unit || workers.find(unit) == workers.end()) { return 0; }

	if (workerJobMap.find(unit) == workerJobMap.end()
		|| workerJobMap[unit] != WorkerData::Minerals) { return 0; }

	if (workerMineralAssignment.find(unit) == workerMineralAssignment.end()) { return 0; }

	auto & patch = workerMineralAssignment[unit];

	if (workersOnMineralPatch.find(patch) == workersOnMineralPatch.end()) { return 0; }

	return workersOnMineralPatch[patch];
}

char WorkerData::getJobCode(BWAPI::Unit unit)
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
	for (auto & mineral : minerals)
	{
		int x = mineral->getPosition().x - 64;
		int y = mineral->getPosition().y - 32;

		if (Config::Debug::DrawWorkerInfo) BWAPI::Broodwar->drawBoxMap(x-2, y-1, x+75, y+14, BWAPI::Colors::Black, true);
		if (Config::Debug::DrawWorkerInfo) BWAPI::Broodwar->drawTextMap(x, y, "\x04 Workers: %d", getNumAssignedWorkers(mineral));
	}
}