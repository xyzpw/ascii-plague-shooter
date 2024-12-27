#include <vector>
#include <optional>
#include "physics/bullet_physics.h"
#include "constants/math_constants.h"
#include "world.h"
#include "position.h"
#include "world_enums.h"
#include "game_utils.h"
#include "random_utils.h"
#include "math_utils.h"
#include "injury_utils.h"
#include "constants/injury_constants.h"
#include "constants/physics_constants.h"

// Return the positions of where a bullet will be when the player shoots.
std::vector<Position> getBulletProjectilePositions(
    World& world, Position position, DIRECTION direction)
{
    bool isVertical = direction == NORTH || direction == SOUTH;
    std::vector<Position> projectilePositions;

    int increase; // Amount of positions to change per loop.

    // Initialize bullet to start at position in front of gun's barrel.
    if (isVertical){
        increase = direction == SOUTH ? +1 : -1;
    }
    else {
        increase = direction == EAST ? +1 : -1;
    }

    while (checkPositionInsideMap(world, position))
    {
        if (isVertical){
            position.row += increase;
        } else {
            position.column += increase;
        }

        projectilePositions.push_back(position);
    }
    return projectilePositions;
}

// Determines where a bullet has impacted the enemy.
std::optional<HIT_LOCATION> getBulletHitLocation(double distance, Firearm firearm)
{
    double decayRate = firearm.accuracyDecay;
    double multiplier = firearm.accuracyScaleFactor;

    double hitProbability = calculateExpDecay(multiplier, decayRate, distance);

    if (!checkProbability(hitProbability)){
        return std::nullopt;
    }

    double pHeadshot = hitProbability * P_HEADSHOT_MULTIPLIER;

    if (checkProbability(pHeadshot)){
        return HIT_LOCATION::HEAD;
    }
    else if (checkProbability(hitProbability)){
        return HIT_LOCATION::THORAX;
    }
    else if (checkProbability(hitProbability)){
        return HIT_LOCATION::ABDOMEN;
    }

    return HIT_LOCATION::LIMBS;
}

int determineShotgunPelletHitCount(int pellets, double spread, double distance)
{
    double area = getSectorWidthAtDistance(distance, spread);
    area *= area;
    area *= PI;

    double hitProbability = 1.0 / area;
    if (hitProbability > 1.0){
        hitProbability = 1.0;
    }

    int hitCount = randBinomialDist(pellets, hitProbability);
    return hitCount;
}
