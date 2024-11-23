#include "common.h"

bool checkBulletWasFatal(HIT_LOCATION, int joules);
int getDelayedDeathTime(HIT_LOCATION);
bool checkShouldDelayedDeath(HIT_LOCATION, bool isHollowPoint);
bool checkIsHindered(HIT_LOCATION);
bool checkExplosionWasFatal(Explosive, double distance);
bool checkExplosionRupturedEardrum(Explosive, double distance);
bool checkExplosionWasHindering(Explosive, double distance);
HIT_LOCATION randHitLocation();
