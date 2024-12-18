#pragma once

#include <vector>
#include <unordered_map>
#include "entities/firearm.h"
#include "entities/explosive.h"
#include "entities/magazine.h"
#include "weapon_enums.h"

struct Inventory{
    std::vector<Firearm> firearms{};
    std::vector<Explosive> explosives{};
    std::vector<Magazine> magazines{};
    std::unordered_map<CARTRIDGE_TYPE, int> ammunition{};
};
