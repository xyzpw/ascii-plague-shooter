#include <iostream>
#include <unistd.h>
#include <ncurses.h>
#include "common.h"

int main()
{
    setlocale(LC_ALL, "");

    World world;
    try{
        playGame(world);
    } catch(std::runtime_error& err){
        endwin();
        printf("\n\n%s\n", err.what());
        getchar();
    }

    return 0;
}
