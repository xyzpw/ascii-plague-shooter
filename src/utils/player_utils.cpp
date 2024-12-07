#include "utils/player_utils.h"
#include "world.h"
#include "entities/player.h"

bool checkPlayerIsDead(World& world, Player& player)
{
    for (auto inf : world.infected){
        if (inf.alive && inf.position == player.position){
            return true;
        }
    }
    return false;
}

void setPlayerSpawnPosition(World& world, Player& player)
{
    player.position.column = (
        world.mapColumnLimits.second - world.mapColumnLimits.first
    ) * 0.5 + world.mapColumnLimits.first;
    player.position.row = (
        world.mapRowLimits.second - world.mapRowLimits.first
    ) * 0.75 + world.mapRowLimits.first;
}
