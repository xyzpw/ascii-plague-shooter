#pragma once

#include "injury_enums.h"
#include "entities/explosive.h"

bool checkBulletWasFatal(HIT_LOCATION, int joules);
int getDelayedDeathTime(HIT_LOCATION);
bool checkShouldDelayedDeath(HIT_LOCATION, bool isHollowPoint);
bool checkShouldHinder(HIT_LOCATION);
bool checkExplosionWasFatal(Explosive, double distance);
bool checkExplosionRupturedEar(Explosive, double distance);
bool checkExplosionWasHindering(Explosive, double distance);
HIT_LOCATION randHitLocation();
bool checkShouldSplatter(HIT_LOCATION, bool isHighVel, int joules,
        double muzzleDistance
);
int calculateDelayedDeathLossRate(HIT_LOCATION,
    bool isHighVelocity, int joules
);
double getBulletExitProbability(CARTRIDGE_TYPE);
