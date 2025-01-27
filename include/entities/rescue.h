#pragma once

#include "position.h"

struct World;

struct Rescue{
    const char* rescueChar = "\u2708";
    Position position;
    bool hasArrived = false;
    bool canBoard = false;
    bool isRescueFinished = false;
    double arrivalEpoch;
    double escapeEpoch;
    void triggerRescueArrival(World& world);
    bool checkHasArrived();
    bool checkIsRescueFinished();
};
