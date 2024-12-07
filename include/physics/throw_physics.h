#pragma once

#include <optional>
#include <vector>
#include "position.h"
#include "world.h"
#include "entities/explosive.h"
#include "world_enums.h"

std::optional<Position> getThrowPosition(World, Explosive, int distance);
std::vector<Position> getThrowPathPositions(
    Position startPos, DIRECTION, Position endPos);
