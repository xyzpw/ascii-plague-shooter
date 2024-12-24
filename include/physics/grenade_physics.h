#pragma once

#include "world.h"
#include "entities/explosive.h"

void processGrenadeThrow(World& world, Explosive grenade, int throwVelocity,
                         int throwAngleDegrees);
