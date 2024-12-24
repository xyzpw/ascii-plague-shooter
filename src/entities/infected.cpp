#include "entities/infected.h"
#include "constants/infected_constants.h"
#include "time_utils.h"

void Infected::markAsDead()
{
    this->alive = false;
    this->infectedChar = INFECTED_CHAR_DEAD;
    this->epochAtDeath = getEpochAsDecimal();
}

void Infected::makeHindered()
{
    if (!isHindered){
        isHindered = true;
        _movementIntervalMs += INFECTED_HINDER_DELAY_MS;
    }
}

void Infected::_updateMovementVars()
{
    this->_lastMovementEpoch = getEpochAsDecimal();
}

// Add --or create-- splatter positions to infected's splatter effect.
void Infected::_updateSplatterEffect(SplatterEffect splatter)
{
    if (!this->splatter.has_value()){
        this->splatter = SplatterEffect{};
    }

    for (auto it : splatter.positions){
        this->splatter->positions.push_back(it);
    }
}
