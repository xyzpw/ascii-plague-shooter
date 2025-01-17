#include <cmath>
#include "injury_utils.h"
#include "injury_enums.h"
#include "probability_utils.h"
#include "random_utils.h"
#include "entities/explosive.h"
#include "math_utils.h"
#include "constants/injury_constants.h"

HIT_LOCATION randHitLocation()
{
    int locationCount = static_cast<int>(HIT_LOCATION::__COUNT);
    return static_cast<HIT_LOCATION>(randIntInRange(0, locationCount));
}

bool checkBulletWasFatal(const HIT_LOCATION location, const int joules)
{
    double pFatal = calculateImpactFatalProbability(location, joules);
    bool wasFatal = checkProbability(pFatal);
    return wasFatal;
}

// Return the rate at which an enemy gets closer to dying from delayed death.
int calculateDelayedDeathLossRate(HIT_LOCATION location, bool isHighVel)
{
    int rate = 0;
    int min, max;
    switch (location){
        case HIT_LOCATION::HEAD:
            min = DELAYED_DEATH_LOSS_RATE_HEAD_MIN;
            max = DELAYED_DEATH_LOSS_RATE_HEAD_MAX;
            break;
        case HIT_LOCATION::THORAX:
            if (isHighVel){
                return randIntInRange(DELAYED_DEATH_LOSS_RATE_THORAX_MAX, 622);
            }
            min = DELAYED_DEATH_LOSS_RATE_THORAX_MIN;
            max = DELAYED_DEATH_LOSS_RATE_THORAX_MAX;
            break;
        case HIT_LOCATION::ABDOMEN:
            if (isHighVel){
                return randIntInRange(DELAYED_DEATH_LOSS_RATE_ABDOMEN_MAX, 200);
            }
            min = DELAYED_DEATH_LOSS_RATE_ABDOMEN_MIN;
            max = DELAYED_DEATH_LOSS_RATE_ABDOMEN_MAX;
            break;
        default:
            min = DELAYED_DEATH_LOSS_RATE_LIMBS_MIN;
            max = DELAYED_DEATH_LOSS_RATE_LIMBS_MAX;
    }
    rate += randIntInRange(min, max);
    return rate;
}

// Check if a bullet should lead to a delayed death.
bool checkShouldDelayedDeath(HIT_LOCATION location, bool isHollowPoint)
{
    switch (location){
        case HIT_LOCATION::HEAD:
            return checkProbability(DELAYED_DEATH_PROBABILITY_HEAD);
        case HIT_LOCATION::THORAX:
            return checkProbability(
                isHollowPoint ? DELAYED_DEATH_PROBABILITY_THORAX_HP :
                                DELAYED_DEATH_PROBABILITY_THORAX
            );
        case HIT_LOCATION::ABDOMEN:
            return checkProbability(
                isHollowPoint ? DELAYED_DEATH_PROBABILITY_ABDOMEN_HP :
                                DELAYED_DEATH_PROBABILITY_ABDOMEN
            );
        default:
            return checkProbability(
                isHollowPoint ? DELAYED_DEATH_PROBABILITY_LIMBS_HP :
                                DELAYED_DEATH_PROBABILITY_LIMBS
            );
    }
}

// Checks if a hit location should be hindering.
bool checkShouldHinder(HIT_LOCATION location)
{
    switch (location){
        case HIT_LOCATION::HEAD:
            return checkProbability(HINDER_PROBABILITY_HEAD);
        case HIT_LOCATION::ABDOMEN:
            return checkProbability(HINDER_PROBABILITY_ABDOMEN);
        case HIT_LOCATION::LIMBS:
            return checkProbability(HINDER_PROBABILITY_LIMBS);
        default:
            return 0;
    }
}

bool checkExplosionWasFatal(Explosive explosive,
                            int fragmentHits, double distance)
{
    double area = computeAreaFromDistance(distance);

    int pascals = computeInverseSquareLaw(explosive.explosionPascals, distance);

    int ke = explosive.fragmentKineticEnergy -
             explosive.fragmentKineticEnergyLossPerMeter * distance;

    double pFragmentLethal = calculateFragmentFatalProbability(ke, fragmentHits);
    double pExplosionLethal = calculateExplosionFatalProbability(pascals);

    bool wasFatal = checkProbability(pFragmentLethal) ||
                    checkProbability(pExplosionLethal);

    return wasFatal;
}

int getFragmentDelayedDeathLossRate(int fragmentHits)
{
    int lossRate = 0;
    for (int i = 0; i < fragmentHits; ++i){
        HIT_LOCATION location = randHitLocation();
        if (checkProbability(DELAYED_DEATH_PROBABILITY_FRAGMENT)){
            lossRate += calculateDelayedDeathLossRate(location, false);
        }
    }
    return lossRate;
}

bool checkExplosionRupturedEar(Explosive explosive, double distance)
{
    int pascalAtDistance = computeInverseSquareLaw(
        explosive.explosionPascals, distance
    );
    bool isClose = calculateEarRuptureProbability(pascalAtDistance);

    return isClose;
}

/*
 Generates a random hit location for each explosion fragment and
 checks if it is hindering.
*/
bool checkExplosionWasHindering(int fragmentHits)
{
    for (int i = 0; i < fragmentHits; ++i){
        HIT_LOCATION rLocation = randHitLocation();
        if (checkShouldHinder(rLocation))
            return true;
    }
    return false;
}

// Checks if the injury results in a splatter effect.
bool checkShouldSplatter(HIT_LOCATION location, bool isHighVel, int joules,
                            double muzzleDistance)
{
    if (joules >= HEADSHOT_SPATTER_REQUIRED_FORCE &&
        location == HIT_LOCATION::HEAD)
    {
        return true;
    }
    else if (isHighVel){
        return muzzleDistance <= 2;
    }

    return false;
}

double getBulletExitProbability(CARTRIDGE_TYPE cartridge)
{
    switch (cartridge){
        case CARTRIDGE_TYPE::CARTRIDGE_22LR:
            return BULLET_EXIT_PROBABILITY_22LR;
        case CARTRIDGE_TYPE::CARTRIDGE_9MM:
            return BULLET_EXIT_PROBABILITY_9MM;
        case CARTRIDGE_TYPE::CARTRIDGE_223_REMINGTON:
        case CARTRIDGE_TYPE::CARTRIDGE_30_06:
            return 1.0;
        default:
            return 0.0;
    }
}

int determineFragmentHitCount(int fragments, double distance)
{
    // Only return half of the initial fragment count if the distance is 0.
    if (distance == 0.0){
        return fragments / 2;
    }

    double area = computeAreaFromDistance(distance);

    double p = 1.0 / area;

    int fragmentCount = randBinomialDist(fragments, p);

    return fragmentCount;
}
