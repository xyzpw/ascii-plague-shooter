#include <vector>
#include <utility>
#include <optional>
#include "world.h"
#include "entities/rescue.h"
#include "entities/supply_drop.h"
#include "entities/firearm.h"
#include "entities/magazine.h"
#include "entities/explosive.h"
#include "weapon_enums.h"
#include "constants/world_constants.h"
#include "constants/firearm_constants.h"
#include "time_utils.h"
#include "random_utils.h"
#include "game_utils.h"

World::World()
{
    std::pair<int, int> terminalSize = getTerminalSize();
    auto limits = getMapLimits(terminalSize);
    mapColumnLimits = limits.first;
    mapRowLimits = limits.second;

    startTime = getEpochAsDecimal();
    nextSupplyDropEpoch = startTime + FIRST_SUPPLY_DROP_DELAY;

    // Set epoch at which rescue will appear.
    rescue.arrivalEpoch = getEpochAsDecimal() + RESCUE_ARRIVAL_ETA;
    rescue.escapeEpoch = rescue.arrivalEpoch + RESCUE_ESCAPE_DURATION;

    infectedSpawner.nextSpawnEpoch = getEpochAsDecimal() + FIRST_INFECTED_SPAWN_DELAY;

    active = true;
}

void World::dropSupplies()
{
    int nextDropTime = 0; // Time until next supply drop

    SupplyDrop drop(mapColumnLimits, mapRowLimits);

    std::vector<Firearm> availableFirearmTypes{};

    for (int i = 0; i != static_cast<int>(FIREARM_TYPE::__COUNT); ++i){
        FIREARM_TYPE type = static_cast<FIREARM_TYPE>(i);
        if (type != FIREARM_TYPE::RUGER_MK_IV){
            availableFirearmTypes.push_back(type);
        }
    }

    Firearm firearm(availableFirearmTypes.at(
        randIntInRange(0, availableFirearmTypes.size() - 1)
    ));
    std::optional<Magazine> magazine;
    int magCount = 0;

    switch (firearm.firearmType){
        case FIREARM_TYPE::GLOCK_17:{
            bool isDoubleStackMag = randIntInRange(0, 1);
            int rounds = isDoubleStackMag ? 17 : 10;

            magazine = Magazine(CARTRIDGE_TYPE::CARTRIDGE_9MM, rounds, rounds);
            magazine->isHollowPoint = !isDoubleStackMag;

            firearm.magazine = magazine.value();
            firearm.loadedRounds = firearm.magazine.cartridgeCount;
            magCount = 2;
            nextDropTime += 50;
            break;
        }
        case FIREARM_TYPE::AR15:
            magazine = Magazine(CARTRIDGE_TYPE::CARTRIDGE_223_REMINGTON, 20, 20);
            magazine->isHollowPoint = checkProbability(0.33);
            if (magazine->isHollowPoint){
                magazine->kineticEnergy = BULLET_KE_223_REMINGTON_HP;
            }

            firearm.magazine = magazine.value();
            magCount = magazine->isHollowPoint ? 3 : 6;

            nextDropTime += 200;
            break;
        case FIREARM_TYPE::BOLT_ACTION_RIFLE:{
            magazine = Magazine(CARTRIDGE_TYPE::CARTRIDGE_30_06, 4, 4);
            magCount = 1;

            // Add additional glock 17 with 2 FMJ double stacked magazines.
            Firearm glock(FIREARM_TYPE::GLOCK_17);
            Magazine _mag = Magazine(CARTRIDGE_TYPE::CARTRIDGE_9MM, 17, 17);
            glock.magazine = _mag;
            glock.magazine.cartridgeCount -= 1;
            for (int i = 0; i < 2; ++i){
                drop.items.magazines.push_back(_mag);
            }
            glock.loadedRounds = glock.magazine.cartridgeCount + 1;
            drop.items.firearms.push_back(glock);

            nextDropTime += 150;
            break;
        }
    }

    drop.items.firearms.push_back(firearm);
    if (magazine.has_value()){
        nextDropTime += magCount * magazine->cartridgeCount / 20;
        for (int i = 0; i < magCount; ++i){
            drop.items.magazines.push_back(magazine.value());
        }
    }

    for (int i = 0; i < randIntInRange(2, 4); ++i){
        drop.items.explosives.push_back(Explosive(EXPLOSIVE_TYPE::M67_GRENADE));
    }

    nextDropTime += drop.items.explosives.size() * 4.5;

    if (randIntInRange(0, 1)){
        drop.items.explosives.push_back(Explosive(EXPLOSIVE_TYPE::M18A1_CLAYMORE));
        nextDropTime += 11;
    }

    supplyDrops.push_back(drop);

    nextSupplyDropEpoch = getEpochAsDecimal() + nextDropTime;
}