#pragma once

#include "world.h"
#include "entities/player.h"

void throwGrenade(World&, Player&, bool isCloseThrow = false);
void plantClaymore(World&, Player&);
