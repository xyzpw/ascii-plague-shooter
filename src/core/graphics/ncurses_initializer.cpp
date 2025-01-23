#include <ncurses.h>
#include "graphics/ncurses_initializer.h"
#include "constants/ncurses_constants.h"

void initializeScreen()
{
    initscr();
    noecho();
    keypad(stdscr, 1);
    curs_set(0);
    use_default_colors();
    nodelay(stdscr, 1);
    start_color();
    init_pair(COLOR_PAIR_SPLATTER, COLOR_ID_SPLATTER, -1);
    init_pair(COLOR_PAIR_GREEN_FG, COLOR_GREEN, -1);
}

void cleanupScreen()
{
    clear();
    refresh();
    endwin();
}
