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
        case FIREARM_TYPE::SIG_M17:{
            magazine = Magazine(CARTRIDGE_TYPE::CARTRIDGE_9MM, 17, 17);
            magazine->isHollowPoint = true;

            firearm.magazine.isHollowPoint = true;

            magCount = 2;
            nextDropTime += magazine->capacity * (magCount + 1) *
                        CARTRIDGE_9MM_HP_COST;
            break;
        }
        case FIREARM_TYPE::AR15:
            magazine = Magazine(CARTRIDGE_TYPE::CARTRIDGE_223_REMINGTON, 20, 20);
            magazine->isHollowPoint = checkProbability(0.5);
            if (magazine->isHollowPoint){
                magazine->kineticEnergy = BULLET_KE_223_REMINGTON_HP;
            }

            firearm.magazine = magazine.value();
            magCount = randIntInRange(1, 3);

            nextDropTime += magazine->isHollowPoint ?
                        magazine->capacity * (magCount + 1) *
                            CARTRIDGE_223_REMINGTON_HP_COST :
                        magazine->capacity * (magCount + 1) *
                            CARTRIDGE_223_REMINGTON_COST;
            break;
        case FIREARM_TYPE::REMINGTON_700:{
            drop.items.ammunition[CARTRIDGE_TYPE::CARTRIDGE_30_06] =
                                  firearm.magazine.capacity;

            nextDropTime += drop.items.ammunition.at(CARTRIDGE_TYPE::CARTRIDGE_30_06);
            nextDropTime += firearm.magazine.capacity;
            nextDropTime *= CARTRIDGE_30_06_COST;

            // Add M17 with 2 additional magazines to the player's inventory.
            Firearm m17(FIREARM_TYPE::SIG_M17);
            Magazine _mag(CARTRIDGE_TYPE::CARTRIDGE_9MM, 17, 17);
            nextDropTime += _mag.capacity * CARTRIDGE_9MM_COST;
            for (int i = 0; i < 2; ++i){
                drop.items.magazines.push_back(_mag);
                nextDropTime += _mag.capacity * CARTRIDGE_9MM_COST;
            }
            drop.items.firearms.push_back(m17);
            break;
        }
    }

    drop.items.firearms.push_back(firearm);
    if (magazine.has_value()){
        for (int i = 0; i < magCount; ++i){
            drop.items.magazines.push_back(magazine.value());
        }
    }

    for (int i = 0; i < randIntInRange(2, 4); ++i){
        drop.items.explosives.push_back(Explosive(EXPLOSIVE_TYPE::M67_GRENADE));
        nextDropTime += 4;
    }

    if (randIntInRange(0, 1)){
        drop.items.explosives.push_back(Explosive(EXPLOSIVE_TYPE::M18A1_CLAYMORE));
        nextDropTime += 8;
    }

    supplyDrops.push_back(drop);

    nextSupplyDropEpoch = getEpochAsDecimal() + nextDropTime;
}