#include <iostream>
#include <unistd.h>
#include "common.h"
#include "audio_handler.h"
#include "graphics/ncurses_initializer.h"
#include "arg_utils.h"

int main(int argc, char **argv)
{
    setlocale(LC_ALL, "");

    World world;
    world.settings.setSettingsFromArgs(parseCmdArgs(argc, argv));

    try{
        playGame(world);
    } catch(std::exception& err){
        cleanupScreen();
        cleanupAudio();
        printf("\nexception: %s\n", err.what());
        printf("Press enter to exit.\n");
        getchar();
    }

    return 0;
}
