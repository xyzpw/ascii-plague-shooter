#pragma once

#include <string>
#include <utility>
#include <optional>
#include <vector>
#include "world_enums.h"
#include "weapon_enums.h"
#include "position.h"

struct Explosive{
    EXPLOSIVE_TYPE explosiveType;
    int explosionPascals;
    double explosionDelay = 0;
    std::string explodeAudioFile = "";
    std::string explodeCloseAudioFile = "";
    Position position;
    const char* explosiveChar = "";
    int fragmentCount;
    int fragmentKineticEnergy;
    double fragmentKineticEnergyLossPerMeter;
    std::optional<int> _explosiveId;
    std::optional<DIRECTION> facingDirection;
    Explosive(EXPLOSIVE_TYPE type);
};