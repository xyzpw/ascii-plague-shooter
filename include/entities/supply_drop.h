#pragma once

#include <utility>
#include "entities/inventory.h"
#include "position.h"

struct SupplyDrop{
    Inventory items;
    Position position;
    const char* itemChar = "$";
    SupplyDrop(std::pair<int, int> colRange, std::pair<int, int> rowRange);
};
