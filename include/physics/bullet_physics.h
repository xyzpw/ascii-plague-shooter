#pragma once

#include <vector>
#include <optional>
#include <map>
#include "position.h"
#include "world.h"
#include "position.h"
#include "world_enums.h"
#include "injury_enums.h"
#include "entities/firearm.h"

std::vector<Position> getBulletProjectilePositions(World&, Position, DIRECTION);
std::vector<std::map<Position, int>> getShotgunPelletTrajectories(
    World&, Position startPos, DIRECTION, Firearm&
);
std::optional<HIT_LOCATION> getBulletHitLocation(double distance, Firearm);
