#include <iostream>
#include <utility>
#include <ctime>
#include "common.h"
#include "math_utils.h"
#include "gameUtilities.h"

World::World()
{
    std::pair<int, int> terminalSize = getTerminalSize();
    auto limits = getMapLimits(terminalSize);
    mapColumnLimits = limits.first;
    mapRowLimits = limits.second;

    startTime = std::time(0);
    nextSupplyDropEpoch = startTime + 20;

    // Epoch in ms at which the infected's positions where updated.
    latestInfectedPositionUpdate = startTime * 1e3;

    active = true;
}

SupplyDrop::SupplyDrop(std::pair<int, int> colRange, std::pair<int, int> rowRange)
{
    int col = std::rand() % (colRange.second - colRange.first + 1) + colRange.first;
    int row = std::rand() % (rowRange.second - rowRange.first + 1) + rowRange.first;
    coordinates = std::make_pair(col, row);
}

void World::dropSupplies()
{
    int nextDropTime = 0; // Time until next supply drop

    SupplyDrop drop(mapColumnLimits, mapRowLimits);
    std::vector<Firearm> availableFirearmTypes{};

    for (int i = 0; i != static_cast<int>(FIREARM_TYPE::__COUNT); ++i){
        FIREARM_TYPE type = static_cast<FIREARM_TYPE>(i);
        if (type != FIREARM_TYPE::RUGER_MK_IV)
            availableFirearmTypes.push_back(type);
    }

    Firearm firearm(
            availableFirearmTypes.at(std::rand() % availableFirearmTypes.size()));
    std::optional<Magazine> magazine;
    int magCount = 0;

    switch (firearm.firearmType){
        case FIREARM_TYPE::GLOCK_17:{
            bool isDoubleStackMag = std::rand() % 2;
            int rounds = isDoubleStackMag ? 17 : 10;
            magazine = Magazine(CARTRIDGE_TYPE::CARTRIDGE_9MM, rounds, rounds);
            magazine->isHollowPoint = std::rand()%5 + 1 <= 2;

            firearm.magazine = magazine.value();
            firearm.loadedRounds = firearm.magazine.cartridgeCount;
            magCount = 2;
            nextDropTime += 50;
            break;
        }
        case FIREARM_TYPE::AR15:
            magazine = Magazine(CARTRIDGE_TYPE::CARTRIDGE_223_REMINGTON, 20, 20);
            magazine->isHollowPoint = std::rand() % 10 == 9;
            if (magazine->isHollowPoint)
                magazine->kineticEnergy = 1300;

            firearm.magazine = magazine.value();
            magCount = magazine->isHollowPoint ? 3 : 6;

            nextDropTime += 200;
            break;
        case FIREARM_TYPE::BOLT_ACTION_RIFLE:{
            magazine = Magazine(CARTRIDGE_TYPE::CARTRIDGE_30_06, 4, 4);
            magCount = 1;

            // Add additional glock 17 with 2 single stacked magazines.
            Firearm glock(FIREARM_TYPE::GLOCK_17);
            glock.magazine.isHollowPoint = true;
            drop.items.firearms.push_back(glock);
            for (int i = 0; i < 2; ++i){
                Magazine _mag = Magazine(CARTRIDGE_TYPE::CARTRIDGE_9MM, 10, 10);
                _mag.isHollowPoint = true;
                drop.items.magazines.push_back(_mag);
            }

            nextDropTime += 150;
            break;
        }
    }

    drop.items.firearms.push_back(firearm);
    if (magazine.has_value()){
        nextDropTime += magCount * magazine->cartridgeCount / 10;
        for (int i = 0; i < magCount; ++i)
            drop.items.magazines.push_back(magazine.value());
    }

    for (int i = 0; i < std::rand() % 3 + 2; ++i)
        drop.items.explosives.push_back(Explosive(EXPLOSIVE_TYPE::M67_GRENADE));
    nextDropTime += drop.items.explosives.size() * 4.5;
    if (std::rand() % 2){
        drop.items.explosives.push_back(Explosive(EXPLOSIVE_TYPE::M18A1_CLAYMORE));
        nextDropTime += 11;
    }

    supplyDrops.push_back(drop);
    nextSupplyDropEpoch = std::time(0) + nextDropTime;
}

std::vector<std::pair<int,int>> getSplatterCoordinates(
        const std::pair<int,int> startCoord, const HIT_LOCATION hitLocation,
        const Magazine magUsed, const DIRECTION direction)
{
    std::vector<std::pair<int,int>> coordinates;
    bool isHorizontal = direction == EAST || direction == WEST;

    int increaseAmount = isHorizontal && direction == EAST ||
            !isHorizontal && direction == SOUTH;
    if (!increaseAmount)
        increaseAmount = -1;

    auto addFromCurrentPos = [&](int xAmount, int yAmount){
        if (xAmount == 0 && yAmount == 0)
            return;
        std::pair<int, int> _coord = startCoord;
        if (xAmount != 0)
            _coord.first += xAmount;
        if (yAmount != 0)
            _coord.second += yAmount;
        coordinates.push_back(_coord);
    };
    auto addSplatterToBothSides = [&](){
        if (isHorizontal){
            addFromCurrentPos(direction == EAST ? 2 : -2, -1);
            addFromCurrentPos(direction == EAST ? 2 : -2, 1);
        }
        else{
            addFromCurrentPos(std::rand()%2 + 1, direction == NORTH ? -2 : 2);
            addFromCurrentPos(0 - (std::rand()%2 + 1),
                    direction == NORTH ? -2 : 2);
        }
    };

    if (isHorizontal)
        addFromCurrentPos(direction == EAST ? 1 : -1, 0);
    else
        addFromCurrentPos(0, direction == NORTH ? -1 : 1);

    if (magUsed.cartridgeType == CARTRIDGE_TYPE::CARTRIDGE_30_06 ||
            magUsed.cartridgeType == CARTRIDGE_TYPE::CARTRIDGE_223_REMINGTON)
    {
        addSplatterToBothSides();
    }
    return coordinates;
}
