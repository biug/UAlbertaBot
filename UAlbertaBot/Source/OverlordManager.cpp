#include "OverlordManager.h"
#include "UnitUtil.h"

using namespace UAlbertaBot;

OverlordManager::OverlordManager() 
{ 
}

void OverlordManager::executeMicro(const BWAPI::Unitset & targets) 
{
    const BWAPI::Unitset & overlordUnits = getUnits();

    for (auto & overlordUnit : overlordUnits)
    {
        // if the order is to attack or defend
        if (order.getType() == SquadOrderTypes::Attack || order.getType() == SquadOrderTypes::Defend) 
        {
            // if we're not near the order position
            if (overlordUnit->getDistance(order.getPosition()) > 100)
            {
                // move to it
                Micro::SmartAttackMove(overlordUnit, order.getPosition());
            }
        }
    }
}