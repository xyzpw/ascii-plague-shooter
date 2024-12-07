#pragma once

#include <unordered_map>

#include "entities/inventory.h"
#include "weapon_enums.h"

int getInventoryMagCount(Inventory, CARTRIDGE_TYPE);
bool checkInventoryHasFirearm(Inventory, FIREARM_TYPE);
std::unordered_map<EXPLOSIVE_TYPE, int> getInventoryExplosiveSize(Inventory);
