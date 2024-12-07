#include <utility>
#include "entities/supply_drop.h"
#include "random_utils.h"

SupplyDrop::SupplyDrop(std::pair<int, int> colRange, std::pair<int, int> rowRange)
{
    position.column = randIntInRange(colRange.first, colRange.second);
    position.row = randIntInRange(rowRange.first, rowRange.second);

    if (position.row - 2 < rowRange.first){
        position.row += 2;
    }
}