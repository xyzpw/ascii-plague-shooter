#include <ncurses.h>
#include "graphics/ncurses_initializer.h"

void initializeScreen()
{
    initscr();
    noecho();
    keypad(stdscr, 1);
    curs_set(0);
    use_default_colors();
    nodelay(stdscr, 1);
}

void cleanupScreen()
{
    clear();
    refresh();
    endwin();
}
