#include "common.h"
#include "game_utilities.h"
#include "infected_handler.h"
#include "math_utils.h"

constexpr double HINDER_PROBABILITY_ABDOMEN = 0.5;
constexpr double HINDER_PROBABILITY_LIMBS = 0.5;
constexpr double HINDER_PROBABILITY_HEAD = 1.0;

HIT_LOCATION randHitLocation()
{
    int locationCount = static_cast<int>(HIT_LOCATION::__COUNT);
    return static_cast<HIT_LOCATION>(randIntInRange(0, locationCount));
}

bool checkBulletWasFatal(const HIT_LOCATION location, const int joules)
{
    double pFatal = computeDeathProbability(location, joules);
    bool wasFatal = checkProbability(pFatal);
    return wasFatal;
}

// Time until bullet causes death if the initial hit was not fatal.
int getDelayedDeathTime(HIT_LOCATION location)
{
    switch (location){
        case HIT_LOCATION::HEAD:{
            return 0;
        }
        case HIT_LOCATION::THORAX:{
            return randIntInRange(2, 10);
        }
        case HIT_LOCATION::ABDOMEN:{
            return randIntInRange(5, 26);
        }
        default: {
            return 85 / 2;
        }
    }
}

// Check if a bullet should lead to a delayed death.
bool checkShouldDelayedDeath(HIT_LOCATION location, bool isHollowPoint)
{
    switch (location){
        case HIT_LOCATION::HEAD:
            return checkProbability(0.9);
        case HIT_LOCATION::THORAX:
            return checkProbability(isHollowPoint ? 0.88 : 0.5);
        case HIT_LOCATION::ABDOMEN:
            return checkProbability(isHollowPoint ? 0.6 : 0.2);
        default:
            return checkProbability(isHollowPoint ? 0.3 : 0.1);
    }
}

// Checks if a hit location should be hindering.
bool checkIsHindered(HIT_LOCATION location)
{
    switch (location){
        case HIT_LOCATION::HEAD:
            // return 1;
            return HINDER_PROBABILITY_HEAD;
        case HIT_LOCATION::ABDOMEN:
            // return checkProbability(0.5);
            return checkProbability(HINDER_PROBABILITY_ABDOMEN);
        case HIT_LOCATION::LIMBS:
            // return checkProbability(0.5);
            return checkProbability(HINDER_PROBABILITY_LIMBS);
        default:
            return 0;
    }
}

bool checkExplosionWasFatal(Explosive explosive, double distance)
{
    double area = computeAreaFromDistance(distance);
    int pascals = computeInverseSquareLaw(explosive.explosionPascals, distance);
    int fragmentCount = explosive.fragmentCount / area;

    int fragmentKe = explosive.fragmentKineticEnergy -
        explosive.fragmentKineticEnergyLossPerMeter * distance;

    double fragmentLethalProb =
            computeFragmentLethalProb(fragmentKe, fragmentCount);
    double explosionLethalProb = computeExplosionLethalProb(pascals);

    bool wasFatal = checkProbability(fragmentLethalProb) ||
                    checkProbability(explosionLethalProb);

    return wasFatal;
}

bool checkExplosionRupturedEardrum(Explosive explosive, double distance)
{
    int pascalAtDistance = computeInverseSquareLaw(
        explosive.explosionPascals, distance
    );
    bool isClose = computeEardrumRuptureProb(pascalAtDistance);

    return isClose;
}

/*
 Generates a random hit location for each explosion fragment and
 checks if it is hindering.
*/
bool checkExplosionWasHindering(Explosive explosive, double distance)
{
    int fragmentCount = explosive.fragmentCount / computeAreaFromDistance(
        distance
    );

    for (int i = 0; i < fragmentCount; ++i)
    {
        HIT_LOCATION loc = randHitLocation();
        if (checkIsHindered(loc))
            return true;
    }

    return false;
}
