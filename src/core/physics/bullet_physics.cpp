#include <vector>
#include <optional>
#include <map>
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

// Return each position and how many pellets would be in that space.
std::vector<std::map<Position, int>> getShotgunPelletTrajectories(
    World& world, Position startPos, DIRECTION direction, Firearm& shotgun)
{
    std::vector<std::map<Position, int>> trajectoryVector;
    // std::map<Position, int> projectilePositions;

    Position activePosition = startPos;

    bool isVertical = direction == NORTH || direction == SOUTH;

    const double SPREAD_DEGREES = shotgun.magazine.pelletSpreadDegrees.value();
    const int PELLET_COUNT = shotgun.magazine.pelletsPerShell.value();

    // Determine increase amount per increment in position.
    int increase;
    if (isVertical){
        increase = direction == SOUTH ? +1 : -1;
    } else{
        increase = direction == EAST ? +1 : -1;
    }

    auto updatePos = [&](){
        if (isVertical)
            activePosition.row += increase;
        else
            activePosition.column += increase;
    };

    // Loop through each position until the end of the map, all while
    // determining the number of pellets in each.
    while (checkPositionInsideMap(world, activePosition))
    {
        updatePos();

        std::map<Position, int> projectilePositions;

        double distance = getPositionDistance(startPos, activePosition);
        double spreadWidth = getSectorWidthAtDistance(distance, SPREAD_DEGREES);
        double spreadRadius = spreadWidth / 2.0;

        // Pellet spread not large enough to reach other positions.
        if (spreadWidth < 1.0){
            projectilePositions[activePosition] = PELLET_COUNT;
            trajectoryVector.push_back(projectilePositions);
            continue;
        }

        // Get each position in a width range.
        std::vector<Position> spreadPositions;
        for (int i = 0; i <= spreadRadius; ++i)
        {
            Position p = activePosition;
            if (isVertical){
                spreadPositions.push_back(
                    Position{.column=p.column + (1 + i), .row=p.row}
                );
                spreadPositions.push_back(
                    Position{.column=p.column - (1 + i), .row=p.row}
                );
            }
            else{
                spreadPositions.push_back(
                    Position{.column=p.column, .row=p.row + (1 + i)}
                );
                spreadPositions.push_back(
                    Position{.column=p.column, .row=p.row - (1 + i)}
                );
            }
        }

        int pelletTrajectorySize = spreadPositions.size();
        int pelletPerWidthPosition = PELLET_COUNT / pelletTrajectorySize;

        // Add each position within width range to the trajectory.
        int pelletsSpent = 0;
        for (auto p : spreadPositions){
            projectilePositions[p] = pelletPerWidthPosition;
            pelletsSpent += pelletPerWidthPosition;
        }

        projectilePositions[activePosition] = PELLET_COUNT - pelletsSpent;
        trajectoryVector.push_back(projectilePositions);
    }

    // return projectilePositions;
    return trajectoryVector;
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
