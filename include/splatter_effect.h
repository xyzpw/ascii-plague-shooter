#pragma once

#include <vector>
#include "position.h"

struct SplatterEffect{
    std::vector<Position> positions;
    const char* splatterChar = "*";
};
