#include <cmath>
#include <algorithm>
#include "common.h"
#include "math_utils.h"
#include "game_utilities.h"

// Probability of bullet wound being lethal.
double computeDeathProbability(HIT_LOCATION location, int joules)
{
    if (joules < MINIMUM_LETHAL_ENERGY)
        return 0;

    double probability;
    switch (location)
    {
        case HIT_LOCATION::HEAD:
            probability = 1 / (1 + std::exp(-0.045 * (joules - 100)));
            break;
        case HIT_LOCATION::THORAX:
            probability = (4.60723 * std::log(joules) - 20.1176) / 10.0;
            break;
        case HIT_LOCATION::ABDOMEN:
            probability = (7.64 * std::log(joules) - 38.52) / 10.0;
            break;
        case HIT_LOCATION::LIMBS:
            probability = (5.83 * std::log(joules) - 32.44) / 10.0;
            break;
    }
    probability = std::clamp(probability, 0.0, 1.0);
    return probability;
}

double computeEardrumRuptureProb(int pascals)
{
    double probability = std::clamp(-12.6 + 1.524 * std::log(pascals), 0.0, 1.0);
    return probability;
}

double computeExplosionLethalProb(int pascals)
{
    double psi = pascals / 6894.8;

    if (psi < 40)
        return 0.0;
    else if (psi >= 92)
        return 1.0;

    double probability = 1 / (1 + std::exp(-0.2088 * (psi - 62)));
    return probability;
}

double computeFragmentLethalProb(int joules, int fragments)
{
    double pFatal = 0.0;

    std::vector<HIT_LOCATION> hitLocations;
    for (int i = 0; i < static_cast<int>(HIT_LOCATION::__COUNT); ++i){
        hitLocations.push_back(static_cast<HIT_LOCATION>(i));
    }

    for (auto location : hitLocations){
        pFatal += computeDeathProbability(location, joules) /
                static_cast<double>(hitLocations.size());
    }

    pFatal = 1 - std::pow(1 - pFatal, fragments);
    return pFatal;
}

HIT_LOCATION getHitLocation(bool isPointBlank)
{
    double pHead = isPointBlank ? 0.33 : 0.1;
    double pThorax = isPointBlank ? 0.33 : 0.35;
    double pAbdomen = isPointBlank ? 0.33 : 0.5;

    if (checkProbability(pHead))
        return HIT_LOCATION::HEAD;
    else if (checkProbability(pHead + pThorax))
        return HIT_LOCATION::THORAX;
    else if (checkProbability(pHead + pThorax + pAbdomen))
        return HIT_LOCATION::ABDOMEN;
    return HIT_LOCATION::LIMBS;
}
