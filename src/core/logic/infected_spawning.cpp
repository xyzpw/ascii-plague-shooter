#include <algorithm>
#include "logic/infected_spawning.h"
#include "position.h"
#include "world.h"
#include "random_utils.h"
#include "time_utils.h"
#include "constants/infected_constants.h"
#include "constants/world_constants.h"

// Returns the column and row to where a new infected should spawn, respectively.
Position getInfectedSpawnPosition(World& world)
{
    int colRange = world.mapColumnLimits.second - world.mapColumnLimits.first;
    int col = randIntInRange(0, colRange) + world.mapColumnLimits.first;

    Position pos = {.column = col, .row = world.mapRowLimits.first};
    return pos;
}

void removeDeadInfected(World& world)
{
    double currentEpoch = getEpochAsDecimal();
    for (auto it = world.infected.begin(); it != world.infected.end();)
    {
        bool hasTimeOfDeath = it->epochAtDeath.has_value();

        if (!it->alive && hasTimeOfDeath &&
            checkHasTimeElapsed(it->epochAtDeath.value(), INFECTED_DEAD_TIME))
        {
            world.infected.erase(it);
        }
        else{
            ++it;
        }
    }
}

void spawnInfected(World& world, int count)
{
    for (int i = 0; i < count; ++i)
    {
        Infected inf{.position = getInfectedSpawnPosition(world)};
        inf._lastMovementEpoch = 0;
        world.infected.push_back(inf);
    }
}

void spawnInfectedGroup(World& world)
{
    double playtime = getEpochAsDecimal() - world.startTime;
    bool isEarly = playtime < EARLY_GAME_TIME_THRESHOLD;

    double spawnDelay = randNormalDist(
        INFECTED_SPAWN_INTERVAL_MEAN,
        INFECTED_SPAWN_INTERVAL_SD
    );
    int spawnSize = randNormalDist(
        INFECTED_SPAWN_SIZE_MEAN,
        INFECTED_SPAWN_SIZE_SD
    );

    spawnDelay = std::clamp(spawnDelay, 1.0, INFECTED_SPAWN_INTERVAL_MAX);
    spawnSize = std::clamp(spawnSize, 1, INFECTED_SPAWN_SIZE_MAX);

    // Increase infected spawn delay at the beginning of the game.
    if (isEarly){
        spawnDelay *= EARLY_GAME_SPAWN_DELAY_MULTIPLIER;
        if (world.infected.size() > 2)
            spawnSize = 0;
    }

    // Ensure only 1 infected spawns if the next infected spawn is within
    // 2 seconds; prevents excessive amounts of infected spawning.
    if (spawnDelay < 2.0)
        spawnSize = 1;

    spawnInfected(world, spawnSize);

    world.infectedSpawner.nextSpawnEpoch = getEpochAsDecimal() + spawnDelay;
}