#pragma once

#include "position.h"

struct InfectedMovement {
    double nextMovementEpoch;
    int movementIntervalMs;
    bool checkShouldMove();
    InfectedMovement();
};

struct InfectedSpawner {
    double nextSpawnEpoch;
    bool checkShouldSpawn();
};

Position getInfectedNextPosition(Position infPos, Position targetPos);
