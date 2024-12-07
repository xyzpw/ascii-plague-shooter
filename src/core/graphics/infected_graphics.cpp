#include <ncurses.h>
#include "graphics/infected_graphics.h"

void drawInfected(World& world)
{
    for (auto inf : world.infected){
        if (inf.splatter.has_value()){
            for (auto pos : inf.splatter->positions){
                mvprintw(pos.row, pos.column, inf.splatter->splatterChar);
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
