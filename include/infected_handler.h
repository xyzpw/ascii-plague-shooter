#pragma once

#include <utility>
#include "common.h"

void updateInfectedPositions(World& world, Player& player);
void handleFirearmShot(World& world, Player& player);
void handleGrenadeExplosion(World& world, Player& player, int explosiveId);
void handleClaymoreExplosion(World& world, Player& player, Explosive explosive);
void removeDeadInfected(World& world);
std::pair<int, int> getInfectedSpawnPosition(World world);
