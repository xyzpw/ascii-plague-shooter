#pragma once

#include <vector>
#include "entities/firearm.h"
#include "entities/explosive.h"
#include "entities/magazine.h"

struct Inventory{
    std::vector<Firearm> firearms{};
    std::vector<Explosive> explosives{};
    std::vector<Magazine> magazines{};
};
