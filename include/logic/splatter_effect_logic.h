#pragma once

#include <vector>
#include "position.h"
#include "injury_enums.h"
#include "world_enums.h"

std::vector<Position> getSplatterPositions(
    Position startPos, HIT_LOCATION location, DIRECTION direction,
    int joules, double muzzleDistance
);