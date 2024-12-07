#pragma once

#include <utility>
#include <vector>
#include "entities/infected.h"
#include "entities/supply_drop.h"
#include "logic/infected_logic.h"
#include "entities/explosive.h"
#include "entities/rescue.h"

struct World{
    double startTime;
    bool active;
    std::pair<int, int> mapColumnLimits{};
    std::pair<int, int> mapRowLimits{};
    std::vector<Infected> infected;
    std::vector<SupplyDrop> supplyDrops{};
    std::vector<Explosive> activeExplosives{};
    InfectedSpawner infectedSpawner;
    InfectedMovement infectedMovement;
    double nextSupplyDropEpoch;
    Rescue rescue;
    void dropSupplies();
    World();
};