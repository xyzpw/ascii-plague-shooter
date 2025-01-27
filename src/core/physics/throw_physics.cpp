#include <vector>
#include <optional>
#include <algorithm>
#include "physics/throw_physics.h"
#include "position.h"
#include "world_enums.h"

// Returns the positions where an object will appear after thrown.
std::vector<Position> getThrowPathPositions(
        Position startPos, DIRECTION direction, Position endPos)
{
    std::vector<Position> pathPositions{};
    int incrementValue = direction == EAST || direction == SOUTH ? +1 : -1;
    bool isVertical = direction == NORTH || direction == SOUTH;

    // Return empty result if the start and end position is the same.
    if ((isVertical && startPos.row == endPos.row) ||
        (!isVertical && startPos.column == endPos.column))
    {
        return pathPositions;
    }

    if (isVertical){
        for (int i = startPos.row + incrementValue; i != endPos.row;
             i += incrementValue)
        {
            Position p;
            p.column = startPos.column;
            p.row = i;
            pathPositions.push_back(p);
        }
    }
    else {
        for (int i = startPos.column + incrementValue; i != endPos.column;
             i += incrementValue)
        {
            Position p;
            p.column = i;
            p.row = startPos.row;
            pathPositions.push_back(p);
        }
    }
    return pathPositions;
}

// Coordinates of where a thrown explosive will land.
std::optional<Position> getThrowPosition(
    World world, Explosive explosive, int distance)
{
    if (!explosive.facingDirection.has_value()){
        return std::nullopt;
    }

    auto direction = explosive.facingDirection.value();

    std::optional<Position> landPosition;

    bool isVertical = direction == NORTH || direction == SOUTH;

    if (isVertical){
        int rowIncrease = direction == SOUTH ? distance : 0 - distance;
        int newRow = std::clamp(
            explosive.position.row + rowIncrease,
            world.mapRowLimits.first,
            world.mapRowLimits.second
        );
        Position temp = {.column = explosive.position.column, .row = newRow};
        landPosition = temp;
    }
    else{
        int colIncrease = direction == EAST ? distance : 0 - distance;
        int newCol = std::clamp(
            colIncrease + explosive.position.column,
            world.mapColumnLimits.first,
            world.mapColumnLimits.second
        );
        Position temp = {.column = newCol, .row = explosive.position.row};
        landPosition = temp;
    }

    return landPosition;
}