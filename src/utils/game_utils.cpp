#include <cmath>
#include <utility>
#include <unistd.h>
#include <sys/ioctl.h>
#include "game_utils.h"
#include "position.h"
#include "world.h"
#include "random_utils.h"

int computePositionChange(Position pos1, Position pos2, bool horizontal)
{
    int diff;

    if (horizontal){
        diff = std::abs(pos1.column - pos2.column);
    }
    else {
        diff = std::abs(pos1.row - pos2.row);
    }

    return diff;
}

// Returns the limits where characters can be move on the terminal.
std::pair<std::pair<int, int>, std::pair<int, int>> getMapLimits(
    std::pair<int, int> termSize)
{
    //NOTE: Column 1 is treated as column 0 with ncurses.
    //NOTE: We want to be 2 spaces from the borders.

    auto [columns, rows] = termSize;
    int centerCol = columns / 2, centerRow = rows / 2;

    int colLeftLimit = std::max(centerCol - 80/2, 3);
    int colRightLimit = std::min(centerCol + 80/2, termSize.first - 2);
    int rowTopLimit = std::max(centerRow - 24/2, 7);
    int rowBottomLimit = std::min(centerRow + 24/2, termSize.second - 2);

    std::pair<std::pair<int, int>, std::pair<int, int>> limits =
        {
            std::make_pair(colLeftLimit - 1, colRightLimit - 1),
            std::make_pair(rowTopLimit - 1, rowBottomLimit - 1),
        };

    return limits;
}

bool checkPositionInsideMap(World world, Position pos)
{
    bool colTooLow = pos.column < world.mapColumnLimits.first;
    bool colTooHigh = pos.column > world.mapColumnLimits.second;
    bool rowTooLow = pos.row < world.mapRowLimits.first;
    bool rowTooHigh = pos.row > world.mapRowLimits.second;

    if (colTooLow || colTooHigh || rowTooLow || rowTooHigh)
        return false;

    return true;
}

std::pair<int, int> getTerminalSize()
{
    struct winsize wSize;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &wSize);
    std::pair<int, int> size = std::make_pair(wSize.ws_col, wSize.ws_row);
    return size;
}

Position getRandMapPosition(World& world)
{
    std::pair<int, int> colLimits = world.mapColumnLimits;
    std::pair<int, int> rowLimits = world.mapRowLimits;

    int col = randIntInRange(colLimits.first, colLimits.second);
    int row = randIntInRange(rowLimits.first, rowLimits.second);

    Position pos{.column=col, .row=row};

    // Prevent position from being near infected spawn.
    Position temp{.column=col, .row=row-2};
    if (!checkPositionInsideMap(world, temp)){
        pos.row += 2;
    }

    return pos;
}
