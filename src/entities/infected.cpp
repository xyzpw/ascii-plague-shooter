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
