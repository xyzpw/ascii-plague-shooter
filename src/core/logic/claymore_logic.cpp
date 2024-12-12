#include <cmath>
#include "logic/claymore_logic.h"
#include "constants/explosive_constants.h"
#include "common.h"

bool checkPosInClaymoreFragmentArea(Explosive, Position);
bool checkClaymoreIsFacingPos(Explosive, Position);

// Return the number of fragments at a given coordinate due to a claymore.
int getClaymoreFragmentCountAtPos(Explosive claymore, Position pos)
{
    bool isProperDirection = checkClaymoreIsFacingPos(claymore, pos);
    bool inFragmentArea = checkPosInClaymoreFragmentArea(claymore, pos);
    if (!isProperDirection || !inFragmentArea){
        return 0;
    }

    Position claymorePos = claymore.position;
    DIRECTION direction = claymore.facingDirection.value();

    bool isHorizontal = direction == EAST || direction == WEST;
    int posDiff = computePositionChange(claymorePos, pos, isHorizontal);
    int sectorArea = computeSectorAreaFromDistance(
        posDiff, CLAYMORE_FRAGMENT_DEGREES
    );

    int fragmentCount = sectorArea == 0 ? claymore.fragmentCount :
                        claymore.fragmentCount / sectorArea;

    if (fragmentCount == 0){
        double p = 1 - std::pow(1 - 1.0/sectorArea, claymore.fragmentCount);
        fragmentCount = checkProbability(p) ? 1 : 0;
    }

    return fragmentCount;
}

// Check if specified coordinates are within the area of a claymores fragments.
bool checkPosInClaymoreFragmentArea(Explosive claymore, Position pos)
{
    if (!claymore.facingDirection.has_value()){
        return 0;
    }

    DIRECTION direction = claymore.facingDirection.value();
    bool isHorizontal = direction == EAST || direction == WEST;

    int posChange = computePositionChange(pos, claymore.position, isHorizontal);
    int posChangeWide = computePositionChange(
        pos, claymore.position, !isHorizontal
    );

    int areaWidth = getSectorWidthAtDistance(
        posChange, CLAYMORE_FRAGMENT_DEGREES
    );

    return posChangeWide <= areaWidth/2;
}

// Check if a claymore is facing the direction at a given coordinate.
bool checkClaymoreIsFacingPos(Explosive claymore, Position pos)
{
    if (!claymore.facingDirection.has_value()){
        return false;
    }

    DIRECTION direction = claymore.facingDirection.value();
    Position claymorePos = claymore.position;

    switch (direction)
    {
        case EAST:
            return pos.column > claymorePos.column;
        case WEST:
            return pos.column < claymorePos.column;
        case NORTH:
            return pos.row < claymorePos.row;
        case SOUTH:
            return pos.row > claymorePos.row;
    }
    return false;
}
