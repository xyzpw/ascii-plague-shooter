#include <vector>
#include "logic/splatter_effect_logic.h"
#include "position.h"
#include "injury_enums.h"
#include "world_enums.h"
#include "random_utils.h"
#include "constants/injury_constants.h"

// Get the positions where splatter will appear.
std::vector<Position> getSplatterPositions(
    Position startPos, HIT_LOCATION location, DIRECTION direction,
    int joules, double muzzleDistance)
{
    std::vector<Position> splatterPositions{};

    bool isHorizontal = direction == EAST || direction == WEST;

    int increaseAmount = isHorizontal && direction == EAST ||
            !isHorizontal && direction == SOUTH;
    if (!increaseAmount){
        increaseAmount = -1;
    }

    auto addFromCurrentPos = [&](int colAmount, int rowAmount){
        if (colAmount == 0 && rowAmount == 0){
            return;
        }
        Position p = startPos;
        if (colAmount != 0){
            p.column += colAmount;
        }
        if (rowAmount != 0){
            p.row += rowAmount;
        }
        splatterPositions.push_back(p);
    };

    auto addSplatterToBothSides = [&](){
        if (isHorizontal){
            addFromCurrentPos(direction == EAST ? 2 : -2, -1);
            addFromCurrentPos(direction == EAST ? 2 : -2, 1);
        }
        else {
            addFromCurrentPos(
                randIntInRange(1, 2),
                direction == NORTH ? -2 : 2
            );
            addFromCurrentPos(
                0 - randIntInRange(1, 2),
                direction == NORTH ? -2 : 2
            );
        }
    };

    if (isHorizontal){
        addFromCurrentPos(direction == EAST ? 1 : -1, 0);
    }
    else{
        addFromCurrentPos(0, direction == NORTH ? -1 : 1);
    }

    // Conditions for larger spatter.
    if (muzzleDistance < 1 || (location == HIT_LOCATION::HEAD &&
        joules >= HEADSHOT_SPATTER_REQUIRED_FORCE))
    {
        addSplatterToBothSides();
        return splatterPositions;
    }

    for (int i = 2; i <= 3; ++i){
        if (isHorizontal)
            addFromCurrentPos(direction == EAST ? i : 0 - i, 0);
        else
            addFromCurrentPos(0, direction == NORTH ? 0 - i : i);
    }
    return splatterPositions;
}
