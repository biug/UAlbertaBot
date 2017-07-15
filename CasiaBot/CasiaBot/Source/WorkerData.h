#pragma once

#include "Common.h"

namespace CasiaBot
{
class WorkerMoveData
{
public:

	int mineralsNeeded;
	int gasNeeded;
	BWAPI::Position position;

	WorkerMoveData(int m, int g, BWAPI::Position p)
        : mineralsNeeded(m)
        , gasNeeded(g)
        , position(p)
	{
		
	}

	WorkerMoveData() {}
};

class WorkerData 
{

public:

	enum WorkerJob {Minerals, Gas, Build, Combat, Idle, Repair, Move, Scout, Default};

private:

	BWAPI::Unitset workers;
	BWAPI::Unitset minerals;
	BWAPI::Unitset refineries;

	std::hash_map<BWAPI::Unit, enum WorkerJob>			workerJobMap;
	std::hash_map<BWAPI::Unit, BWAPI::Unit>				workerMineralMap;
	std::hash_map<BWAPI::Unit, BWAPI::Unit>				workerRefineryMap;
	std::hash_map<BWAPI::Unit, BWAPI::Unit>				workerRepairMap;
	std::hash_map<BWAPI::Unit, WorkerMoveData>			workerMoveMap;
	std::hash_map<BWAPI::Unit, BWAPI::UnitType>			workerBuildingTypeMap;
	std::hash_map<BWAPI::Unit, std::set<BWAPI::Unit>>	workersInMineral;
	std::hash_map<BWAPI::Unit, std::set<BWAPI::Unit>>	workersInRefinery;

	std::hash_map<BWAPI::Unit, int>			mineralWorkerCount;
	std::hash_map<BWAPI::Unit, int>			refineryWorkerCount;

	std::hash_map<BWAPI::Unit, int>			workersOnMineralPatch;
	std::hash_map<BWAPI::Unit, BWAPI::Unit>	workerMineralAssignment;

	void clearPreviousJob(BWAPI::Unit unit);

public:

	WorkerData();

	void					checkResources();
	void					workerDestroyed(BWAPI::Unit unit);
	void					addMineral(BWAPI::Unit unit);
	void					addRefinery(BWAPI::Unit unit);
	void					removeMineral(BWAPI::Unit unit);
	void					removeRefinery(BWAPI::Unit unit);
	void					addWorker(BWAPI::Unit unit);
	void					addWorker(BWAPI::Unit unit, WorkerJob job, BWAPI::Unit jobUnit);
	void					addWorker(BWAPI::Unit unit, WorkerJob job, BWAPI::UnitType jobUnitType);
	void					setWorkerJob(BWAPI::Unit unit, WorkerJob job, BWAPI::Unit jobUnit);
	void					setWorkerJob(BWAPI::Unit unit, WorkerJob job, WorkerMoveData wmd);
	void					setWorkerJob(BWAPI::Unit unit, WorkerJob job, BWAPI::UnitType jobUnitType);

	int						getNumWorkers() const;
	int						getNumMineralWorkers() const;
	int						getNumGasWorkers() const;
	int						getNumIdleWorkers() const;
	char					getJobCode(BWAPI::Unit unit);

	void					getMineralWorkers(std::set<BWAPI::Unit> & mw);
	void					getGasWorkers(std::set<BWAPI::Unit> & mw);
	void					getBuildingWorkers(std::set<BWAPI::Unit> & mw);
	void					getRepairWorkers(std::set<BWAPI::Unit> & mw);
	
	bool					mineralIsFull(BWAPI::Unit mineral);
	int						numMineralNeedWorker(BWAPI::Unit mineral);
	int						getMineralsNearDepot(BWAPI::Unit mineral);

	bool					isMineralPatchEnough();
	int						getNumAssignedWorkers(BWAPI::Unit unit);
	int						getNumAssignedWorkersOnPatch(BWAPI::Unit unit);
	BWAPI::Unit				getMineralToMine(BWAPI::Unit worker);
	BWAPI::Unit				getLarvaDepot();

	enum WorkerJob	getWorkerJob(BWAPI::Unit unit);
	BWAPI::Unit		getWorkerResource(BWAPI::Unit unit);
	BWAPI::Unit		getWorkerMineral(BWAPI::Unit unit);
	BWAPI::Unit		getMineralWorker(BWAPI::Unit unit);
	BWAPI::Unit		getRefineryWorker(BWAPI::Unit unit);
	BWAPI::Unit		getWorkerRepairUnit(BWAPI::Unit unit);
	BWAPI::UnitType	getWorkerBuildingType(BWAPI::Unit unit);
	WorkerMoveData	getWorkerMoveData(BWAPI::Unit unit);

    BWAPI::Unitset          getMineralPatchesNearDepot(BWAPI::Unitset bases);
    void                    addToMineralPatch(BWAPI::Unit unit, int num);
	void					drawMineralDebugInfo();

	const BWAPI::Unitset & getWorkers() const { return workers; }

};
}