#include <ncurses.h>
#include <string>
#include "graphics/map_graphics.h"
#include "world.h"

void drawMapLimitBorders(World& world)
{
    auto colLimits = world.mapColumnLimits;
    auto rowLimits = world.mapRowLimits;
    std::string vertBorder(
            (colLimits.second + 2) - (colLimits.first - 2), '#');
    mvprintw(rowLimits.first - 2, colLimits.first - 2, vertBorder.c_str());
    mvprintw(rowLimits.second + 2, colLimits.first - 2, vertBorder.c_str());
    for (int row = rowLimits.first - 2; row <= rowLimits.second + 2; ++row){
        mvprintw(row, colLimits.first - 2, "#");
        mvprintw(row, colLimits.second + 2, "#");
    }
}

void clearMap(World& world)
{
        auto colLimits = world.mapColumnLimits;
        auto rowLimits = world.mapRowLimits;
        for (int row = rowLimits.first - 1; row <= rowLimits.second + 1; ++row){
            for (int col = colLimits.first - 1; col <= colLimits.second + 1;
                    ++col)
            {
                move(row, col);
                printw(" ");
            }
        }
}
