#include <optional>
#include <thread>
#include "logic/infected_motion.h"
#include "world.h"
#include "entities/player.h"
#include "random_utils.h"
#include "time_utils.h"
#include "utils/inventory_utils.h"
#include "logic/infected_handler.h"

std::optional<Explosive> getInfectedTriggeredMine(World&, Infected&);

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

        // Check --and handle-- if an infected has stepped on a mine.
        std::optional<Explosive> triggeredMine = getInfectedTriggeredMine(
            world, inf
        );
        if (triggeredMine.has_value()){
            std::thread(
                handleM16MineExplosion, std::ref(world),
                std::ref(player), triggeredMine.value()
            ).detach();
        }
    }
}

std::optional<Explosive> getInfectedTriggeredMine(World& world, Infected& inf)
{
    for (auto& exp : world.activeExplosives)
    {
        bool isMine = exp.explosiveType == EXPLOSIVE_TYPE::M16_MINE;
        if (!isMine || !exp.isTriggerable.has_value()){
            continue;
        }

        if (exp.position == inf.position && exp.isTriggerable.value()){
            // Prevent mine from being triggered more than once.
            exp.isTriggerable = false;

            return exp;
        }
    }
    return std::nullopt;
}
