#pragma once

#include "world.h"
#include "position.h"

void playExplosionAnimation(World&, Position, double energy);
void playExplosionAnimationThread(World&, Position, double energy);
