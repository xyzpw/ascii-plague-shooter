#include "logic/infected_motion.h"
#include "world.h"
#include "entities/player.h"
#include "random_utils.h"
#include "time_utils.h"

// Updates each infected's location to follow a player.
void updateInfectedPositions(World& world, Player& player)
{
    if (!player.alive)
        return;

    for (auto& inf : world.infected)
    {
        if (!checkHasTimeElapsed(
            inf._lastMovementEpoch,
            static_cast<double>(inf._movementIntervalMs) / 1000))
        {
            continue;
        }
        inf._updateMovementVars();

        if (!inf.alive || inf.position == player.position)
            continue;

        Position nextPos = getInfectedNextPosition(inf.position, player.position);

        // Do not update infected's position if there is another infected
        // in that space.
        bool canMove = true;
        for (auto other : world.infected){
            if (other.alive && nextPos == other.position){
                canMove = false;
            }
        }

        if (canMove){
            inf.position = nextPos;
        }
    }
}
