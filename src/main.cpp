#include <iostream>
#include <unistd.h>
#include <ncurses.h>
#include "common.h"
#include "arg_utils.h"

int main(int argc, char **argv)
{
    setlocale(LC_ALL, "");

    World world;
    world.settings.setSettingsFromArgs(parseCmdArgs(argc, argv));

    try{
        playGame(world);
    } catch(std::runtime_error& err){
        endwin();
        printf("\n\n%s\n", err.what());
        getchar();
    }

    return 0;
}
