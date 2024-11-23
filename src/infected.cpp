#include <ctime>
#include "common.h"

#define DEAD_INFECTED_CHAR "D"

void Infected::markAsDead()
{
    this->alive = false;
    this->infectedChar = DEAD_INFECTED_CHAR;
    this->timeOfDeath = std::time(0);
}
