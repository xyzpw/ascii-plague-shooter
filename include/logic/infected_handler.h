#pragma once

#include "world.h"
#include "entities/player.h"
#include "entities/explosive.h"

void handleFirearmShot(World& world, Player& player);
void handleShotgunPelletShot(World&, Player&);
void handleGrenadeExplosion(World& world, Player& player, int explosiveId);
void handleClaymoreExplosion(World& world, Player& player, Explosive explosive);
void handleDelayedDeathInfected(World&, Player&);
