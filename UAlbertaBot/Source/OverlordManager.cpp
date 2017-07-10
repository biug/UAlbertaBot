#include "OverlordManager.h"
#include "UnitUtil.h"

using namespace UAlbertaBot;

OverlordManager::OverlordManager() 
{ 
}
void OverlordManager::executeMicro(const BWAPI::Unitset & targets)
{
}
void OverlordManager::executeMove(const SquadOrder & inputOrder) 
{
    const BWAPI::Unitset & overlordUnits = getUnits();
    //different position need to do
    BWTA::BaseLocation * enemyBaseLocation = InformationManager::Instance().getMainBaseLocation(BWAPI::Broodwar->enemy());

    for (auto & overlordUnit : overlordUnits)
    {
        // if we're not near the order position
        if (overlordUnit->getDistance(enemyBaseLocation->getPosition()) > 100)
        {
            // move to it
            Micro::SmartMove(overlordUnit, enemyBaseLocation->getPosition());
        }
    }
}