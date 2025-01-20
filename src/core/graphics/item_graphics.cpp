#include <ncurses.h>
#include "graphics/item_graphics.h"
#include "entities/rescue.h"

void drawWorldItems(World& world)
{
    for (auto drop : world.supplyDrops){
        mvprintw(drop.position.row, drop.position.column, drop.itemChar);
    }

    // Display explosives.
    for (auto explosive : world.activeExplosives){
        mvprintw(
            explosive.position.row,
            explosive.position.column,
            explosive.explosiveChar
        );
    }
}

void drawRescue(World& world)
{
    Rescue rescue = world.rescue;
    mvprintw(rescue.position.row, rescue.position.column, rescue.rescueChar);
}
