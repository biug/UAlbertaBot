#include "Common.h"
#include "WorkerManager.h"
#include "Micro.h"

using namespace CasiaBot;

WorkerManager::WorkerManager() 
{
	needLessGas = needMoreGas = needLessMineral = needMoreMineral = false;
    previousClosestWorker = nullptr;
}

WorkerManager & WorkerManager::Instance() 
{
	static WorkerManager instance;
	return instance;
}

void WorkerManager::update() 
{
	updateResourceStatus();
	updateWorkerStatus();
	handleGasWorkers();
	handleIdleWorkers();
	handleMineralWorkers();
	handleMoveWorkers();
	handleCombatWorkers();

	drawResourceDebugInfo();
	drawWorkerInformation(450,20);

	workerData.drawMineralDebugInfo();
}

void WorkerManager::updateResourceStatus()
{
	if (BWAPI::Broodwar->getFrameCount() % 10 == 0)
	{
		needLessGas = needMoreGas = needLessMineral = needMoreMineral = false;
		gasNotUsed = false;
		gasUsed.push_back(BWAPI::Broodwar->self()->spentGas());
		if (gasUsed.size() >= 20)
		{
			if (gasUsed.front() == gasUsed.back()) {
				gasNotUsed = true;
			}
			gasUsed.pop_front();
		}
		int mineral = BWAPI::Broodwar->self()->minerals();
		int gas = BWAPI::Broodwar->self()->gas();
		if (mineral < 50)
		{
			needMoreMineral = true;
		}
		if (gas > 448 && gasNotUsed)
		{
			needLessGas = true;
		}
		else if (gas < 100 || !gasNotUsed)
		{
			needMoreGas = true;
		}
	}
}

void WorkerManager::updateWorkerStatus() 
{
	for (auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		// add the depot if it exists
		if (unit->getType().isResourceDepot())
		{
			workerData.addMineral(unit);
		}

		if (unit->getType().isRefinery())
		{
			workerData.addRefinery(unit);
		}

		// if something morphs into a worker, add it
		if (unit->getType().isWorker() && unit->getHitPoints() >= 0)
		{
			//BWAPI::Broodwar->printf("A worker was shown %d", unit->getID());
			workerData.addWorker(unit);
		}
	}
	// for each of our Workers
	for (auto & worker : workerData.getWorkers())
	{
		if (!worker->isCompleted())
		{
			continue;
		}

		if (BWAPI::Broodwar->getFrameCount() > 10 && BWAPI::Broodwar->getFrameCount() % 10 == 0)
		{
			if (worker->getType().isWorker())
			{
				workersPos[worker].push_back(worker->getPosition());
				if (workersPos[worker].size() >= 12)
				{
					auto job = workerData.getWorkerJob(worker);
					// ����ǲɿ�ũ������120֡û��λ�ƣ�˵���������ˣ�Ӧ������ΪIdle
					if (isWorkerIdle(worker))
					{
						workerData.setWorkerJob(worker, WorkerData::Idle, nullptr);
					}
					workersPos[worker].pop_front();
				}
			}
		}

		// if it's idle
		if (worker->isIdle() && 
			(workerData.getWorkerJob(worker) != WorkerData::Build) && 
			(workerData.getWorkerJob(worker) != WorkerData::Move) &&
			(workerData.getWorkerJob(worker) != WorkerData::Scout)) 
		{
			workerData.setWorkerJob(worker, WorkerData::Idle, nullptr);
		}

		// if its job is gas
		if (workerData.getWorkerJob(worker) == WorkerData::Gas)
		{
			BWAPI::Unit refinery = workerData.getWorkerResource(worker);

			// if the refinery doesn't exist anymore
			if (!refinery || !refinery->exists() ||	refinery->getHitPoints() <= 0)
			{
				workerData.setWorkerJob(worker, WorkerData::Idle, nullptr);
			}
		}

		// ���Idleũ���λ����Ϣ�����½���
		if (workerData.getWorkerJob(worker) == WorkerData::Idle)
		{
			workersPos[worker].clear();
		}
	}
}

void WorkerManager::setRepairWorker(BWAPI::Unit worker, BWAPI::Unit unitToRepair)
{
    workerData.setWorkerJob(worker, WorkerData::Repair, unitToRepair);
}

void WorkerManager::stopRepairing(BWAPI::Unit worker)
{
    workerData.setWorkerJob(worker, WorkerData::Idle, nullptr);
}

void WorkerManager::handleGasWorkers()
{
	// for each unit we have
	bool firstRefinery = true;
	for (auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		// if that unit is a refinery
		if (unit->getType().isRefinery() && unit->isCompleted() && !isGasStealRefinery(unit))
		{
			if (needLessGas)
			{
				BWAPI::Unit gasWorker = workerData.getRefineryWorker(unit);
				if (gasWorker != nullptr && gasWorker->isMoving())
				{
					setMineralWorker(gasWorker);
				}
			}
			else
			{
				// get the number of workers currently assigned to it
				int workersNeeded = Config::Macro::WorkersPerRefinery
					- workerData.getNumAssignedWorkers(unit);
				if (!firstRefinery && !needMoreGas) workersNeeded == 0;

				// if it's less than we want it to be, fill 'er up
				for (int i = 0; i < workersNeeded; ++i)
				{
					BWAPI::Unit gasWorker = getGasWorker(unit);
					if (gasWorker)
					{
						workerData.setWorkerJob(gasWorker, WorkerData::Gas, unit);
					}
				}
				firstRefinery = false;
			}
		}
	}
}

bool WorkerManager::isGasStealRefinery(BWAPI::Unit unit)
{
    BWTA::BaseLocation * enemyBaseLocation = InformationManager::Instance().getMainBaseLocation(BWAPI::Broodwar->enemy());
    if (!enemyBaseLocation)
    {
        return false;
    }
    
    if (enemyBaseLocation->getGeysers().empty())
    {
        return false;
    }
    
	for (auto & u : enemyBaseLocation->getGeysers())
	{
        if (unit->getTilePosition() == u->getTilePosition())
        {
            return true;
        }
	}

    return false;
}

void WorkerManager::handleMineralWorkers()
{
	if (!workerData.isMineralPatchEnough()) { return; }
	for (auto & worker : workerData.getWorkers())
	{
		CAB_ASSERT(worker != nullptr, "Worker was null");

		if (workerData.getWorkerJob(worker) == WorkerData::Minerals)
		{
			int numWorkersOnPatch = workerData.getNumAssignedWorkersOnPatch(worker);
			if (numWorkersOnPatch > 2)
			{
				setMineralWorker(worker);
			}
		}
	}
}

void WorkerManager::handleIdleWorkers() 
{
	// for each of our workers
	for (auto & worker : workerData.getWorkers())
	{
        CAB_ASSERT(worker != nullptr, "Worker was null");

		// if it is idle
		if (workerData.getWorkerJob(worker) == WorkerData::Idle) 
		{
			// send it to the nearest mineral patch
			setMineralWorker(worker);
		}
	}
}

// bad micro for combat workers
void WorkerManager::handleCombatWorkers()
{
	for (auto & worker : workerData.getWorkers())
	{
        CAB_ASSERT(worker != nullptr, "Worker was null");

		if (workerData.getWorkerJob(worker) == WorkerData::Combat)
		{
			BWAPI::Broodwar->drawCircleMap(worker->getPosition().x, worker->getPosition().y, 4, BWAPI::Colors::Yellow, true);
			BWAPI::Unit target = getClosestEnemyUnit(worker);

			if (target)
			{
				Micro::SmartAttackUnit(worker, target);
			}
		}
	}
}

BWAPI::Unit WorkerManager::getClosestEnemyUnit(BWAPI::Unit worker)
{
    CAB_ASSERT(worker != nullptr, "Worker was null");

	BWAPI::Unit closestUnit = nullptr;
	double closestDist = 10000;

	for (auto & unit : BWAPI::Broodwar->enemy()->getUnits())
	{
		double dist = unit->getDistance(worker);

		if ((dist < 400) && (!closestUnit || (dist < closestDist)))
		{
			closestUnit = unit;
			closestDist = dist;
		}
	}

	return closestUnit;
}

void WorkerManager::finishedWithCombatWorkers()
{
	for (auto & worker : workerData.getWorkers())
	{
        CAB_ASSERT(worker != nullptr, "Worker was null");

		if (workerData.getWorkerJob(worker) == WorkerData::Combat)
		{
			setMineralWorker(worker);
		}
	}
}

BWAPI::Unit WorkerManager::getClosestMineralWorkerTo(BWAPI::Unit enemyUnit)
{
    CAB_ASSERT(enemyUnit != nullptr, "enemyUnit was null");

    BWAPI::Unit closestMineralWorker = nullptr;
    double closestDist = 100000;

    if (previousClosestWorker)
    {
        if (previousClosestWorker->getHitPoints() > 0)
        {
            return previousClosestWorker;
        }
    }

    // for each of our workers
	for (auto & worker : workerData.getWorkers())
	{
        CAB_ASSERT(worker != nullptr, "Worker was null");
		if (!worker)
		{
			continue;
		}
		// if it is a move worker
        if (workerData.getWorkerJob(worker) == WorkerData::Minerals) 
		{
			double dist = worker->getDistance(enemyUnit);

            if (!closestMineralWorker || dist < closestDist)
            {
                closestMineralWorker = worker;
                dist = closestDist;
            }
		}
	}

    previousClosestWorker = closestMineralWorker;
    return closestMineralWorker;
}

BWAPI::Unit WorkerManager::getWorkerScout()
{
    // for each of our workers
	for (auto & worker : workerData.getWorkers())
	{
        CAB_ASSERT(worker != nullptr, "Worker was null");
		if (!worker)
		{
			continue;
		}
		// if it is a move worker
        if (workerData.getWorkerJob(worker) == WorkerData::Scout) 
		{
			return worker;
		}
	}

    return nullptr;
}

BWAPI::Unit WorkerManager::getLarvaDepot()
{
	return workerData.getLarvaDepot();
}

void WorkerManager::handleMoveWorkers() 
{
	// for each of our workers
	for (auto & worker : workerData.getWorkers())
	{
        CAB_ASSERT(worker != nullptr, "Worker was null");

		// if it is a move worker
		if (workerData.getWorkerJob(worker) == WorkerData::Move) 
		{
			WorkerMoveData data = workerData.getWorkerMoveData(worker);
			
			Micro::SmartMove(worker, data.position);
		}
	}
}

bool WorkerManager::isWorkerIdle(BWAPI::Unit unit)
{
	if (unit && workersPos.find(unit) == workersPos.end())
	{
		auto & poses = workersPos[unit];
		for (int i = 1; i < poses.size(); ++i)
		{
			if (poses[i - 1] != poses[i])
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

// set a worker to mine minerals
void WorkerManager::setMineralWorker(BWAPI::Unit unit)
{
    CAB_ASSERT(unit != nullptr, "Unit was null");

	// update workerData with the new job
	workerData.setWorkerJob(unit, WorkerData::Minerals, nullptr);
}

BWAPI::Unit WorkerManager::getClosestResource(BWAPI::Unit worker, BWAPI::Unitset & poses)
{
	CAB_ASSERT(worker != nullptr, "Worker was null");

	BWAPI::Unit closestDepot = nullptr;
	double closestDistance = 0;

	for (auto & unit : poses)
	{
		CAB_ASSERT(unit != nullptr, "Unit was null");

		double distance = unit->getDistance(worker);
		if (!closestDepot || distance < closestDistance)
		{
			closestDepot = unit;
			closestDistance = distance;
		}
	}

	return closestDepot;
}

// other managers that need workers call this when they're done with a unit
void WorkerManager::finishedWithWorker(BWAPI::Unit unit) 
{
	CAB_ASSERT(unit != nullptr, "Unit was null");

	//BWAPI::Broodwar->printf("BuildingManager finished with worker %d", unit->getID());
	if (workerData.getWorkerJob(unit) != WorkerData::Scout)
	{
		workerData.setWorkerJob(unit, WorkerData::Idle, nullptr);
	}
}

BWAPI::Unit WorkerManager::getGasWorker(BWAPI::Unit refinery)
{
	CAB_ASSERT(refinery != nullptr, "Refinery was null");

	BWAPI::Unit closestWorker = nullptr;
	double closestDistance = 0;

	for (auto & unit : workerData.getWorkers())
	{
        CAB_ASSERT(unit != nullptr, "Unit was null");

		if (workerData.getWorkerJob(unit) == WorkerData::Minerals)
		{
			double distance = unit->getDistance(refinery);
			if (!closestWorker || distance < closestDistance)
			{
				closestWorker = unit;
				closestDistance = distance;
			}
		}
	}

	return closestWorker;
}

 void WorkerManager::setBuildingWorker(BWAPI::Unit worker, Building & b)
 {
     CAB_ASSERT(worker != nullptr, "Worker was null");

     workerData.setWorkerJob(worker, WorkerData::Build, b.type);
 }

// gets a builder for BuildingManager to use
// if setJobAsBuilder is true (default), it will be flagged as a builder unit
// set 'setJobAsBuilder' to false if we just want to see which worker will build a building
BWAPI::Unit WorkerManager::getBuilder(Building & b, bool setJobAsBuilder)
{
	// variables to hold the closest worker of each type to the building
	BWAPI::Unit closestMovingWorker = nullptr;
	BWAPI::Unit closestMiningWorker = nullptr;
	double closestMovingWorkerDistance = 0;
	double closestMiningWorkerDistance = 0;

	// look through each worker that had moved there first
	for (auto & unit : workerData.getWorkers())
	{
        CAB_ASSERT(unit != nullptr, "Unit was null");

        // gas steal building uses scout worker
        if (b.isGasSteal && (workerData.getWorkerJob(unit) == WorkerData::Scout))
        {
            if (setJobAsBuilder)
            {
                workerData.setWorkerJob(unit, WorkerData::Build, b.type);
            }
            return unit;
        }

		// mining worker check
		if (unit->isCompleted() && (workerData.getWorkerJob(unit) == WorkerData::Minerals))
		{
			// if it is a new closest distance, set the pointer
			double distance = unit->getDistance(BWAPI::Position(b.finalPosition));
			if (!closestMiningWorker || distance < closestMiningWorkerDistance)
			{
				closestMiningWorker = unit;
				closestMiningWorkerDistance = distance;
			}
		}

		// moving worker check
		if (unit->isCompleted() && (workerData.getWorkerJob(unit) == WorkerData::Move))
		{
			// if it is a new closest distance, set the pointer
			double distance = unit->getDistance(BWAPI::Position(b.finalPosition));
			if (!closestMovingWorker || distance < closestMovingWorkerDistance)
			{
				closestMovingWorker = unit;
				closestMovingWorkerDistance = distance;
			}
		}
	}

	// if we found a moving worker, use it, otherwise using a mining worker
	BWAPI::Unit chosenWorker = closestMovingWorker ? closestMovingWorker : closestMiningWorker;

	// if the worker exists (one may not have been found in rare cases)
	if (chosenWorker && setJobAsBuilder)
	{
		workerData.setWorkerJob(chosenWorker, WorkerData::Build, b.type);
	}

	// return the worker
	return chosenWorker;
}

// sets a worker as a scout
void WorkerManager::setScoutWorker(BWAPI::Unit worker)
{
	CAB_ASSERT(worker != nullptr, "Worker was null");

	workerData.setWorkerJob(worker, WorkerData::Scout, nullptr);
}

// gets a worker which will move to a current location
BWAPI::Unit WorkerManager::getMoveWorker(BWAPI::Position p)
{
	// set up the pointer
	BWAPI::Unit closestWorker = nullptr;
	double closestDistance = 0;

	// for each worker we currently have
	for (auto & unit : workerData.getWorkers())
	{
        CAB_ASSERT(unit != nullptr, "Unit was null");

		// only consider it if it's a mineral worker
		if (unit->isCompleted() && workerData.getWorkerJob(unit) == WorkerData::Minerals)
		{
			// if it is a new closest distance, set the pointer
			double distance = unit->getDistance(p);
			if (!closestWorker || distance < closestDistance)
			{
				closestWorker = unit;
				closestDistance = distance;
			}
		}
	}

	// return the worker
	return closestWorker;
}

// sets a worker to move to a given location
void WorkerManager::setMoveWorker(int mineralsNeeded, int gasNeeded, BWAPI::Position p)
{
	// set up the pointer
	BWAPI::Unit closestWorker = nullptr;
	double closestDistance = 0;

	// for each worker we currently have
	for (auto & unit : workerData.getWorkers())
	{
        CAB_ASSERT(unit != nullptr, "Unit was null");

		// only consider it if it's a mineral worker
		if (unit->isCompleted() && workerData.getWorkerJob(unit) == WorkerData::Minerals)
		{
			// if it is a new closest distance, set the pointer
			double distance = unit->getDistance(p);
			if (!closestWorker || distance < closestDistance)
			{
				closestWorker = unit;
				closestDistance = distance;
			}
		}
	}

	if (closestWorker)
	{
		//BWAPI::Broodwar->printf("Setting worker job Move for worker %d", closestWorker->getID());
		workerData.setWorkerJob(closestWorker, WorkerData::Move, WorkerMoveData(mineralsNeeded, gasNeeded, p));
	}
	else
	{
		//BWAPI::Broodwar->printf("Error, no worker found");
	}
}

// will we have the required resources by the time a worker can travel a certain distance
bool WorkerManager::willHaveResources(int mineralsRequired, int gasRequired, double distance)
{
	// if we don't require anything, we will have it
	if (mineralsRequired <= 0 && gasRequired <= 0)
	{
		return true;
	}

	// the speed of the worker unit
	double speed = BWAPI::Broodwar->self()->getRace().getWorker().topSpeed();

    CAB_ASSERT(speed > 0, "Speed is negative");

	// how many frames it will take us to move to the building location
	// add a second to account for worker getting stuck. better early than late
	double framesToMove = (distance / speed) + 50;

	// magic numbers to predict income rates
	double mineralRate = getNumMineralWorkers() * 0.045;
	double gasRate     = getNumGasWorkers() * 0.07;

	// calculate if we will have enough by the time the worker gets there
	if (mineralRate * framesToMove >= mineralsRequired + 50 &&
		gasRate * framesToMove >= gasRequired + 24)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void WorkerManager::setCombatWorker(BWAPI::Unit worker)
{
	CAB_ASSERT(worker != nullptr, "Worker was null");

	workerData.setWorkerJob(worker, WorkerData::Combat, nullptr);
}

void WorkerManager::onUnitMorph(BWAPI::Unit unit)
{
	CAB_ASSERT(unit != nullptr, "Unit was null");

	// if something morphs into a worker, add it
	if (unit->getType().isWorker() && unit->getPlayer() == BWAPI::Broodwar->self() && unit->getHitPoints() >= 0)
	{
		workerData.addWorker(unit);
	}

	// if something morphs into a building, it was a worker?
	if (unit->getType().isBuilding() && unit->getPlayer() == BWAPI::Broodwar->self())
	{
		//BWAPI::Broodwar->printf("A Drone started building");
		workerData.workerDestroyed(unit);
	}
}

void WorkerManager::onUnitShow(BWAPI::Unit unit)
{
	CAB_ASSERT(unit != nullptr, "Unit was null");

	// add the depot if it exists
	if (unit->getType().isResourceDepot() && unit->getPlayer() == BWAPI::Broodwar->self())
	{
		workerData.addMineral(unit);
	}

	// add the refinery if it exists
	if (unit->getType().isRefinery() && unit->getPlayer() == BWAPI::Broodwar->self())
	{
		workerData.addRefinery(unit);
	}

	// if something morphs into a worker, add it
	if (unit->getType().isWorker() && unit->getPlayer() == BWAPI::Broodwar->self() && unit->getHitPoints() >= 0)
	{
		//BWAPI::Broodwar->printf("A worker was shown %d", unit->getID());
		workerData.addWorker(unit);
	}
}


void WorkerManager::rebalanceWorkers()
{
	// for each worker
	for (auto & worker : workerData.getWorkers())
	{
        CAB_ASSERT(worker != nullptr, "Worker was null");

		if (!workerData.getWorkerJob(worker) == WorkerData::Minerals)
		{
			continue;
		}

		BWAPI::Unit mineral = workerData.getWorkerMineral(worker);

		if (mineral && workerData.mineralIsFull(mineral))
		{
			workerData.setWorkerJob(worker, WorkerData::Idle, nullptr);
		}
		else if (!mineral)
		{
			workerData.setWorkerJob(worker, WorkerData::Idle, nullptr);
		}
	}
}

void WorkerManager::onUnitDestroy(BWAPI::Unit unit) 
{
	CAB_ASSERT(unit != nullptr, "Unit was null");

	if (unit->getType().isResourceDepot() && unit->getPlayer() == BWAPI::Broodwar->self())
	{
		workerData.removeMineral(unit);
	}

	if (unit->getType().isRefinery() && unit->getPlayer() == BWAPI::Broodwar->self())
	{
		workerData.removeRefinery(unit);
	}

	if (unit->getType().isWorker() && unit->getPlayer() == BWAPI::Broodwar->self()) 
	{
		workerData.workerDestroyed(unit);
	}

	if (unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field)
	{
		rebalanceWorkers();
	}
}

void WorkerManager::drawResourceDebugInfo() 
{
    if (!Config::Debug::DrawResourceInfo)
    {
        return;
    }

	for (auto & worker : workerData.getWorkers()) 
    {
        CAB_ASSERT(worker != nullptr, "Worker was null");

		char job = workerData.getJobCode(worker);

		BWAPI::Position pos = worker->getTargetPosition();

		BWAPI::Broodwar->drawTextMap(worker->getPosition().x, worker->getPosition().y - 5, "\x07%c", job);

		BWAPI::Broodwar->drawLineMap(worker->getPosition().x, worker->getPosition().y, pos.x, pos.y, BWAPI::Colors::Cyan);

		BWAPI::Unit mineral = workerData.getWorkerMineral(worker);
		if (mineral)
		{
			BWAPI::Broodwar->drawLineMap(worker->getPosition().x, worker->getPosition().y, mineral->getPosition().x, mineral->getPosition().y, BWAPI::Colors::Orange);
		}
	}
}

void WorkerManager::drawWorkerInformation(int x, int y) 
{
    if (!Config::Debug::DrawWorkerInfo)
    {
        return;
    }

	BWAPI::Broodwar->drawTextScreen(x, y, "\x04 Workers %d", workerData.getNumMineralWorkers());
	BWAPI::Broodwar->drawTextScreen(x, y+20, "\x04 UnitID");
	BWAPI::Broodwar->drawTextScreen(x+50, y+20, "\x04 State");

	int yspace = 0;

	for (auto & unit : workerData.getWorkers())
	{
        CAB_ASSERT(unit != nullptr, "Worker was null");

		BWAPI::Broodwar->drawTextScreen(x, y+40+((yspace)*10), "\x03 %d", unit->getID());
		BWAPI::Broodwar->drawTextScreen(x+50, y+40+((yspace++)*10), "\x03 %c", workerData.getJobCode(unit));
	}
}

bool WorkerManager::isFree(BWAPI::Unit worker)
{
    CAB_ASSERT(worker != nullptr, "Worker was null");

	return workerData.getWorkerJob(worker) == WorkerData::Minerals || workerData.getWorkerJob(worker) == WorkerData::Idle;
}

bool WorkerManager::isWorkerScout(BWAPI::Unit worker)
{
    CAB_ASSERT(worker != nullptr, "Worker was null");

	return (workerData.getWorkerJob(worker) == WorkerData::Scout);
}

bool WorkerManager::isBuilder(BWAPI::Unit worker)
{
    CAB_ASSERT(worker != nullptr, "Worker was null");

	return (workerData.getWorkerJob(worker) == WorkerData::Build);
}

int WorkerManager::getNumMineralWorkers() 
{
	return workerData.getNumMineralWorkers();	
}

int WorkerManager::getNumIdleWorkers() 
{
	return workerData.getNumIdleWorkers();	
}

int WorkerManager::getNumGasWorkers() 
{
	return workerData.getNumGasWorkers();
}
