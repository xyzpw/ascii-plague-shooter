#pragma once

#include "weapon_enums.h"

struct Magazine{
    CARTRIDGE_TYPE cartridgeType;
    int cartridgeCount;
    int capacity;
    bool isHollowPoint = false;
    int kineticEnergy;
    double kineticEnergyLossPerMeter;
    bool isHighVelocity; // Bullet travels faster than 2,000 ft/s.
    int penetrateEnergyThreshold; // joules required to penetrate enemy
    Magazine(CARTRIDGE_TYPE cartridge = CARTRIDGE_TYPE::__COUNT,
            int maxRounds = 0, int rounds = 0);
};