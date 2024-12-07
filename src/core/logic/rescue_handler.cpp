#include "logic/rescue_handler.h"
#include "world.h"
#include "entities/player.h"
#include "entities/rescue.h"
#include "graphics/item_graphics.h"
#include "math_utils.h"

// Handles how to react to rescue at the current loop in the main game loop.
void handleRescueGameLoop(World& world, Player& player)
{
    Rescue *rescue = &world.rescue;
    if (!rescue->hasArrived && rescue->checkHasArrived()){
        rescue->triggerRescueArrival(world);
    }
    else if (rescue->hasArrived && rescue->canBoard && !rescue->isRescueFinished){
        drawRescue(world);

        if (getPositionDistance(player.position, rescue->position) <= 2){
            player.isRescued = true;
            player.gameStats.setEndGameMessage(GAME_END_MSG_RESCUED);
            world.active = false;
        }
        rescue->isRescueFinished = rescue->checkIsRescueFinished();
    }
    else if (rescue->isRescueFinished && !player.isRescued){
        rescue->canBoard = false;
        player.gameStats.setEndGameMessage(GAME_END_MSG_RESCUE_FAILED);
        world.active = false;
    }
}
