#pragma once

#include <utility>
#include "position.h"
#include "world.h"

std::pair<std::pair<int, int>, std::pair<int, int>> getMapLimits(
    std::pair<int, int> termSize
);

std::pair<int, int> getTerminalSize();

int computePositionChange(Position pos1, Position pos2, bool horizontal);

bool checkPositionInsideMap(World, Position);

Position getRandMapPosition(World& world);
