#pragma once

#include <vector>
#include <optional>
#include "position.h"
#include "world.h"
#include "position.h"
#include "world_enums.h"
#include "injury_enums.h"
#include "entities/firearm.h"

std::vector<Position> getBulletProjectilePositions(World&, Position, DIRECTION);
std::optional<HIT_LOCATION> getBulletHitLocation(double distance, Firearm);
