#include "common.h"
#include "game_utilities.h"
#include "infected_handler.h"

void Rescue::triggerRescueArrival(World& world)
{
    hasArrived = true;
    coordinates = getRandMapCoordinates(world);
    canBoard = true;
}
