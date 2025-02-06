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
    int explosionEnergy;
    double explosionDelay = 0;
    std::string explodeAudioFile = "";
    std::string explodeCloseAudioFile = "";
    Position position;
    const char* explosiveChar = "";
    int fragmentCount;
    int fragmentKineticEnergy;
    double fragmentKineticEnergyLossPerMeter;
    double fragmentPenetrateEnergyThreshold;
    std::optional<int> _explosiveId;
    std::optional<DIRECTION> facingDirection;
    std::optional<bool> isTriggerable;
    Explosive(EXPLOSIVE_TYPE type);
};
