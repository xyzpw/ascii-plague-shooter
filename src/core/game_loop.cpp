#include <string>
#include <ncurses.h>
#include <utility>
#include <vector>
#include <thread>
#include <ctime>
#include <sstream>
#include "common.h"
#include "physics/grenade_physics.h"
#include "logic/infected_handler.h"
#include "controls/input/keyboard_input.h"
#include "graphics/map_graphics.h"
#include "graphics/infected_graphics.h"
#include "graphics/player_graphics.h"
#include "graphics/text_graphics.h"
#include "graphics/map_graphics.h"
#include "graphics/item_graphics.h"
#include "graphics/ncurses_initializer.h"
#include "audio_handler.h"
#include "logic/infected_spawning.h"
#include "logic/infected_motion.h"
#include "logic/rescue_handler.h"

void playGame(World world)
{
    // Reduce time until rescue if quick game is enabled.
    if (world.settings.quickGame){
        double subtractAmount = RESCUE_ARRIVAL_ETA / 2.0;
        world.rescue.arrivalEpoch -= subtractAmount;
        world.rescue.escapeEpoch -= subtractAmount;
    }

    initializeAudio();
    Player myPlayer;

    myPlayer.inventory.magazines.push_back(Magazine(
            CARTRIDGE_TYPE::CARTRIDGE_22LR, 10, 10));

    // Add coordinates to player.
    setPlayerSpawnPosition(world, myPlayer);
    myPlayer.fixWeaponAppearance();

    initializeScreen();

    // Erase screen and draw map borders for game.
    erase();
    drawMapLimitBorders(world);

    while (world.active)
    {
        clearMap(world);

        int capturedKey = getch();
        respondToKeyPress(world, myPlayer, capturedKey);

        // Spawn infected.
        if (world.infectedSpawner.checkShouldSpawn()){
            spawnInfectedGroup(world);
        }

        // Drop supplies at intervals.
        if (world.nextSupplyDropEpoch <= getEpochAsDecimal()){
            world.dropSupplies();
        }

        if (checkPlayerIsDead(world, myPlayer)){
            world.active = false;
        }

        handleDelayedDeathInfected(world, myPlayer);
        if (world.infectedMovement.checkShouldMove())
        {
            double nextUpdate = static_cast<double>(INFECTED_MOVEMENT_INTERVAL_MS);
            world.infectedMovement.nextMovementEpoch =
                getEpochAsDecimal() + static_cast<double>(INFECTED_MOVEMENT_INTERVAL_MS) / 1e3;
            updateInfectedPositions(world, myPlayer);
        }

        // Display stuff.
        removeDeadInfected(world);
        drawWorldItems(world);
        drawInfected(world);
        drawPlayer(myPlayer);
        drawGameStatus(world, myPlayer);
        drawRescueCountdown(world);

        // Handle rescue outcome and display rescue.
        handleRescueGameLoop(world, myPlayer);

        refresh();
        napms(16);

        // End game if player is not alive.
        if (!myPlayer.alive){
            world.active = false;
        }
    }

    std::thread(cleanupAudio).detach();
    displayEndGame(world, myPlayer);

    bool hasQuit = false;
    while (!hasQuit){
        auto key = getch();
        if (key == QUIT_KEY){
            hasQuit = true;
        }
        napms(25);
    }

    cleanupScreen();
}
