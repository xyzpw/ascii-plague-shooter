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

    auto makeFirearmAndMagHollowPoint = [&](){
        firearm.magazine.ammoType = AMMO_TYPE::HOLLOW_POINT;
        firearm.chamberRoundType = AMMO_TYPE::HOLLOW_POINT;
        if (magazine.has_value()){
            magazine->ammoType = AMMO_TYPE::HOLLOW_POINT;
        }
    };

    switch (firearm.firearmType){
        case FIREARM_TYPE::SIG_M17:{
            magazine = Magazine(CARTRIDGE_TYPE::CARTRIDGE_9MM, 17, 17);

            makeFirearmAndMagHollowPoint();

            magCount = randIntInRange(1, 2);
            nextDropTime += magazine->capacity * (magCount + 1) *
                        CARTRIDGE_9MM_HP_COST;
            break;
        }
        case FIREARM_TYPE::AR15:
            magazine = Magazine(CARTRIDGE_TYPE::CARTRIDGE_223_REMINGTON, 20, 20);
            if (checkProbability(0.5)){
                makeFirearmAndMagHollowPoint();
                magazine->kineticEnergy = BULLET_KE_223_REMINGTON_HP;
            }

            firearm.magazine = magazine.value();
            magCount = randIntInRange(1, 2);

            nextDropTime = magazine->capacity * (magCount + 1);
            if (magazine->ammoType == AMMO_TYPE::HOLLOW_POINT){
                nextDropTime *= CARTRIDGE_223_REMINGTON_HP_COST;
            }
            else {
                nextDropTime *= CARTRIDGE_223_REMINGTON_COST;
            }
            break;
        case FIREARM_TYPE::REMINGTON_700:{
            drop.items.ammunition[CARTRIDGE_TYPE::CARTRIDGE_30_06] =
                                  firearm.magazine.capacity * randIntInRange(1, 3);

            nextDropTime += drop.items.ammunition.at(CARTRIDGE_TYPE::CARTRIDGE_30_06);
            nextDropTime += firearm.magazine.capacity;
            nextDropTime *= CARTRIDGE_30_06_COST;
            break;
        }
        case FIREARM_TYPE::BENELLI_M4:{
            bool useSlugs = checkProbability(0.5);
            CARTRIDGE_TYPE cartridge = useSlugs ?
                                    CARTRIDGE_TYPE::CARTRIDGE_12GA_SLUG :
                                    CARTRIDGE_TYPE::CARTRIDGE_12GA_BUCKSHOT;
            drop.items.ammunition[cartridge] =
                                  firearm.magazine.capacity * randIntInRange(1,2);

            firearm.magazine = Magazine(
                cartridge,
                firearm.magazine.capacity,
                firearm.magazine.capacity - 1
            );
            firearm.cartridgeType = cartridge;

            if (useSlugs){
                firearm.chamberRoundType = AMMO_TYPE::RIFLED_SLUG;
            }

            nextDropTime += drop.items.ammunition.at(cartridge);
            nextDropTime += firearm.magazine.capacity;
            nextDropTime *= CARTRIDGE_BUCKSHOT_COST;
            break;
        }
        case FIREARM_TYPE::DB_SHOTGUN:{
            bool useSlugs = checkProbability(0.5);
            CARTRIDGE_TYPE cartridge = useSlugs ?
                           CARTRIDGE_TYPE::CARTRIDGE_12GA_SLUG :
                           CARTRIDGE_TYPE::CARTRIDGE_12GA_BUCKSHOT;

            firearm.cartridgeType = cartridge;
            firearm.magazine = Magazine(cartridge, 2, 2);
            if (useSlugs){
                firearm.chamberRoundType = AMMO_TYPE::RIFLED_SLUG;
            }

            drop.items.ammunition[cartridge] = randIntInRange(2, 5) * 2;

            nextDropTime += drop.items.ammunition.at(cartridge) + 2;
            nextDropTime *= CARTRIDGE_BUCKSHOT_COST;
            break;
        }
    }

    drop.items.firearms.push_back(firearm);

    if (magazine.has_value()){
        for (int i = 0; i < magCount; ++i){
            drop.items.magazines.push_back(magazine.value());
        }
    }

    // Add grenade.
    int supplyDropGrenadeCount = randIntInRange(2, 4);
    for (int i = 0; i < supplyDropGrenadeCount; ++i){
        drop.items.explosives.push_back(Explosive(EXPLOSIVE_TYPE::M67_GRENADE));
        nextDropTime += 4;
    }

    // Add claymore.
    if (randIntInRange(0, 1)){
        drop.items.explosives.push_back(
            Explosive(EXPLOSIVE_TYPE::M18A1_CLAYMORE)
        );
        nextDropTime += 8;
    }

    // Add M16 mine.
    if (randIntInRange(0, 1)){
        drop.items.explosives.push_back(Explosive(EXPLOSIVE_TYPE::M16_MINE));
        nextDropTime += 4;
    }

    supplyDrops.push_back(drop);

    nextSupplyDropEpoch = getEpochAsDecimal() + nextDropTime;
}

bool World::checkShouldDropSupplies()
{
    return this->nextSupplyDropEpoch <= getEpochAsDecimal();
}
