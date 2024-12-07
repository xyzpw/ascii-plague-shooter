#include "entities/rescue.h"
#include "world.h"
#include "game_utils.h"
#include "time_utils.h"

void Rescue::triggerRescueArrival(World& world)
{
    hasArrived = true;
    position = getRandMapPosition(world);
    canBoard = true;
}

bool Rescue::checkHasArrived()
{
    if (getEpochAsDecimal() >= arrivalEpoch){
        return true;
    }
    return false;
}

bool Rescue::checkIsRescueFinished()
{
    if (getEpochAsDecimal() >= escapeEpoch){
        return true;
    }
    return false;
}
