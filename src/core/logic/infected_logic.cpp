#include "logic/infected_logic.h"
#include "constants/infected_constants.h"
#include "time_utils.h"
#include "position.h"

bool InfectedSpawner::checkShouldSpawn()
{
    return getEpochAsDecimal() >= nextSpawnEpoch;
}

InfectedMovement::InfectedMovement()
{
    this->nextMovementEpoch = getEpochAsDecimal();
    this->movementIntervalMs = INFECTED_MOVEMENT_INTERVAL_MS;
}

bool InfectedMovement::checkShouldMove()
{
    return getEpochAsDecimal() >= nextMovementEpoch;
}

Position getInfectedNextPosition(Position infPos, Position targetPos)
{
    int deltaCol = targetPos.column - infPos.column;
    int deltaRow = targetPos.row - infPos.row;

    if (deltaCol < 0)
        infPos.column -= 1;
    else if (deltaCol > 0)
        infPos.column += 1;
    if (deltaRow > 0)
        infPos.row += 1;
    else if (deltaRow < 0)
        infPos.row -= 1;

    return infPos;
}
