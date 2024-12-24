#pragma once

#include <optional>
#include "position.h"
#include "splatter_effect.h"
#include "constants/infected_constants.h"
#include "constants/injury_constants.h"

struct Infected{
    const char* infectedChar = INFECTED_CHAR;

    bool alive = true;
    bool isHindered = false;

    std::optional<double> delayedDeathStartEpoch;
    std::optional<double> epochAtDeath; // Epoch at which the infected has died.

    int delayedDeathCounter = DELAYED_DEATH_COUNTER_MAX;
    int delayedDeathLossRate = 0;

    std::optional<SplatterEffect> splatter;

    double _lastMovementEpoch;
    int _movementIntervalMs = INFECTED_MOVEMENT_INTERVAL_MS;
    Position position;

    void markAsDead();
    void _updateMovementVars();
    void _updateSplatterEffect(SplatterEffect);
    void makeHindered();
};