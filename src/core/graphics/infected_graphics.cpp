#include <ncurses.h>
#include "graphics/infected_graphics.h"
#include "constants/ncurses_constants.h"

void drawInfected(World& world)
{
    for (auto inf : world.infected)
    {
        if (inf.splatter.has_value())
        {
            for (auto pos : inf.splatter->positions)
            {
                mvprintw(pos.row, pos.column, inf.splatter->splatterChar);

                // Change splatter to red if colors are enabled.
                if (world.settings.colors){
                    mvchgat(pos.row, pos.column, 1, A_NORMAL,
                            COLOR_PAIR_SPLATTER, nullptr
                    );
                }
            }
        }
    }

    for (auto inf : world.infected){
        if (!inf.alive){
            mvprintw(inf.position.row, inf.position.column, inf.infectedChar);
        }
    }

    for (auto inf : world.infected){
        if (inf.alive){
            mvprintw(inf.position.row, inf.position.column, inf.infectedChar);
        }
    }
}
