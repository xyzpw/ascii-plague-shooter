#include <cmath>
#include <algorithm>
#include <vector>
#include "probability_utils.h"
#include "injury_enums.h"
#include "constants/injury_constants.h"
#include "random_utils.h"

// Probability of a bullet being lethal on impact.
double calculateImpactFatalProbability(HIT_LOCATION location, int joules)
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
            probability = (4.61 * std::log(joules) - 20.12) / 10.0;
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

double calculateEarRuptureProbability(int pascals)
{
    pascals *= 0.1; // multiply by 100 ms

    double probability = -12.6 + 1.524 * std::log(pascals);
    probability = std::clamp(probability, 0.0, 1.0);
    return probability;
}

double calculateExplosionFatalProbability(int pascals)
{
    double psi = pascals / 6894.8;

    if (psi < 40)
        return 0.0;
    else if (psi >= 92)
        return 1.0;

    double probability = 1 / (1 + std::exp(-0.2088 * (psi - 62)));
    return probability;
}

double calculateFragmentFatalProbability(int joules, int fragments)
{
    double pFatal = 0.0;

    std::vector<HIT_LOCATION> hitLocations;
    for (int i = 0; i < static_cast<int>(HIT_LOCATION::__COUNT); ++i){
        hitLocations.push_back(static_cast<HIT_LOCATION>(i));
    }

    for (auto location : hitLocations){
        pFatal += calculateImpactFatalProbability(location, joules) /
                static_cast<double>(hitLocations.size());
    }

    pFatal = 1 - std::pow(1 - pFatal, fragments);
    return pFatal;
}
