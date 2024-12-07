#include <ncurses.h>
#include "graphics/player_graphics.h"

void drawPlayer(Player& player)
{
    mvprintw(player.position.row, player.position.column, player.playerChar);
    mvprintw(
        player.weaponPosition.row,
        player.weaponPosition.column,
        player.weaponChar
    );
}
