#include <utility>
#include "common.h"
#include "game_utilities.h"
#include "math_utils.h"

#define CLAYMORE_FRAGMENT_DEGREES 60

constexpr int M67_PASCALS = 962e3;
constexpr int CLAYMORE_PASCALS = 4556e3;
constexpr int M67_FRAGMENT_COUNT = 1300;
constexpr int CLAYMORE_FRAGMENT_COUNT = 700;
constexpr int M67_FRAGMENT_KE = 105;
constexpr int CLAYMORE_FRAGMENT_KE = 467;
constexpr double M67_FRAGMENT_KE_LOSS = 0.47;
constexpr double CLAYMORE_FRAGMENT_KE_LOSS = 2.2;


Explosive::Explosive(EXPLOSIVE_TYPE type)
{
    explosiveType = type;
    switch (type)
    {
        case EXPLOSIVE_TYPE::M67_GRENADE:
            explosionPascals = M67_PASCALS;
            explosionDelay = randIntInRange(40, 55) / 10.0;
            explodeAudioFile = "explosion_with_debris.wav";
            explodeCloseAudioFile = "explosion_close.wav";
            explosiveChar = "\u2022";
            fragmentCount = M67_FRAGMENT_COUNT;
            fragmentKineticEnergy = M67_FRAGMENT_KE;
            fragmentKineticEnergyLossPerMeter = M67_FRAGMENT_KE_LOSS;
            break;
        case EXPLOSIVE_TYPE::M18A1_CLAYMORE:
            explosionPascals = CLAYMORE_PASCALS;
            explosionDelay = 1;
            explodeAudioFile = "explosion.wav";
            explodeCloseAudioFile = "explosion_close.wav";
            explosiveChar = "<";
            fragmentCount = CLAYMORE_FRAGMENT_COUNT;
            fragmentKineticEnergy = CLAYMORE_FRAGMENT_KE;
            fragmentKineticEnergyLossPerMeter = CLAYMORE_FRAGMENT_KE_LOSS;
            break;
    }
}

bool checkPosInClaymoreFragmentArea(Explosive, std::pair<int, int>);
bool checkClaymoreIsFacingPos(Explosive, std::pair<int, int>);

// Return the number of fragments at a given coordinate due to a claymore.
int getClaymoreFragmentCountAtPos(Explosive claymore, std::pair<int, int> pos)
{
    bool isProperDirection = checkClaymoreIsFacingPos(claymore, pos);
    bool inFragmentArea = checkPosInClaymoreFragmentArea(claymore, pos);
    if (!isProperDirection || !inFragmentArea){
        return 0;
    }

    std::pair<int, int> claymorePos = claymore.coordinates;
    DIRECTION direction = claymore.facingDirection.value();

    bool isHorizontal = direction == EAST || direction == WEST;
    int posDiff = computeCoordinatesChange(claymorePos, pos, isHorizontal);
    int sectorArea = computeSectorAreaFromDistance(
        posDiff, CLAYMORE_FRAGMENT_DEGREES
    );

    int fragmentCount = sectorArea == 0 ? claymore.fragmentCount :
                        claymore.fragmentCount / sectorArea;

    return fragmentCount;
}

// Check if specified coordinates are within the area of a claymores fragments.
bool checkPosInClaymoreFragmentArea(Explosive claymore, std::pair<int, int> pos)
{
    if (!claymore.facingDirection.has_value()){
        return 0;
    }

    DIRECTION direction = claymore.facingDirection.value();
    bool isHorizontal = direction == EAST || direction == WEST;

    int coordChange = computeCoordinatesChange(
        pos, claymore.coordinates, isHorizontal
    );
    int coordChangeWide = computeCoordinatesChange(
        pos, claymore.coordinates, !isHorizontal
    );

    int areaWidth = getSectorWidthAtDistance(
        coordChange, CLAYMORE_FRAGMENT_DEGREES
    );

    return coordChangeWide <= areaWidth/2;
}

// Check if a claymore is facing the direction at a given coordinate.
bool checkClaymoreIsFacingPos(Explosive claymore, const std::pair<int, int> pos)
{
    if (!claymore.facingDirection.has_value()){
        return false;
    }

    DIRECTION direction = claymore.facingDirection.value();
    std::pair<int, int> claymorePos = claymore.coordinates;

    switch (direction)
    {
        case EAST:
            return pos.first > claymorePos.first;
        case WEST:
            return pos.first < claymorePos.first;
        case NORTH:
            return pos.second < claymorePos.second;
        case SOUTH:
            return pos.second > claymorePos.second;
    }
    return false;
}