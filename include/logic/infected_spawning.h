#include "world.h"
#include "position.h"

Position getInfectedSpawnPosition(World& world);

void removeDeadInfected(World& world);
void spawnInfected(World&, int count);
void spawnInfectedGroup(World&);