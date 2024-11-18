#include <iostream>
#include <unistd.h>
#include <utility>
#include "common.h"
#include "gameUtilities.h"
#include "infected_handler.h"

void Rescue::triggerRescueArrival(World& world)
{
    hasArrived = true;
    coordinates = getRandMapCoordinates(world);
    canBoard = true;
}
