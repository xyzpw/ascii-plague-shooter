#pragma once

#include <string>
#include "weapon_enums.h"
#include "entities/magazine.h"

struct Firearm{
    FIREARM_TYPE firearmType;
    std::string name;
    int magazineCapacity;
    bool isChambered = true;
    bool canShoot = true;

    double reloadTime;
    double fastReloadTime;
    double chamberReloadDelay;
    int shootIntervalMs;
    double isReloading = false;
    std::string shootAudioFile = "";
    double accuracyDecay;
    double accuracyScaleFactor;
    CARTRIDGE_TYPE cartridgeType;
    Magazine magazine;
    int loadedRounds;
    Firearm(FIREARM_TYPE type);
};