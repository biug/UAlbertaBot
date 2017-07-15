#pragma once

#include <Common.h>
#include "BuildingManager.h"
#include "WorkerData.h"

namespace CasiaBot
{
class Building;

class WorkerManager
{
    WorkerData  workerData;
	BWAPI::Unit previousClosestWorker;
	bool		needLessGas;
	bool		needMoreGas;
	bool		needLessMineral;
	bool		needMoreMineral;
	bool		gasNotUsed;

	std::deque<int>										gasUsed;
	std::map<BWAPI::Unit, std::deque<BWAPI::Position>>	workersPos;

    void        setMineralWorker(BWAPI::Unit unit);
    bool        isGasStealRefinery(BWAPI::Unit unit);
    
    void        handleIdleWorkers();
    void        handleGasWorkers();
    void        handleMoveWorkers();
    void        handleCombatWorkers();

    WorkerManager();

public:

    void        update();
    void        onUnitDestroy(BWAPI::Unit unit);
    void        onUnitMorph(BWAPI::Unit unit);
    void        onUnitShow(BWAPI::Unit unit);
    void        onUnitRenegade(BWAPI::Unit unit);
    void        finishedWithWorker(BWAPI::Unit unit);

    void        finishedWithCombatWorkers();

    void        drawResourceDebugInfo();
    void        updateWorkerStatus();
	void		updateResourceStatus();
    void        drawWorkerInformation(int x,int y);

    int         getNumMineralWorkers();
    int         getNumGasWorkers();
    int         getNumIdleWorkers();
    void        setScoutWorker(BWAPI::Unit worker);

    bool        isWorkerScout(BWAPI::Unit worker);
    bool        isFree(BWAPI::Unit worker);
    bool        isBuilder(BWAPI::Unit worker);

    BWAPI::Unit getBuilder(Building & b,bool setJobAsBuilder = true);
    BWAPI::Unit getMoveWorker(BWAPI::Position p);
    BWAPI::Unit getClosestDepot(BWAPI::Unit worker);
    BWAPI::Unit getGasWorker(BWAPI::Unit refinery);
    BWAPI::Unit getClosestEnemyUnit(BWAPI::Unit worker);
    BWAPI::Unit getClosestMineralWorkerTo(BWAPI::Unit enemyUnit);
    BWAPI::Unit getWorkerScout();

    void        setBuildingWorker(BWAPI::Unit worker,Building & b);
    void        setRepairWorker(BWAPI::Unit worker,BWAPI::Unit unitToRepair);
    void        stopRepairing(BWAPI::Unit worker);
    void        setMoveWorker(int m,int g,BWAPI::Position p);
    void        setCombatWorker(BWAPI::Unit worker);

    bool        willHaveResources(int mineralsRequired,int gasRequired,double distance);
    void        rebalanceWorkers();

    static WorkerManager &  Instance();
};
}