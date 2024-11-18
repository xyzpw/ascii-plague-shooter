#include <iostream>
#include <string>
#include <ncurses.h>
#include <utility>
#include <vector>
#include <unistd.h>
#include <thread>
#include <ctime>
#include <sstream>
#include "common.h"
#include "gameUtilities.h"
#include "physics_manager.h"
#include "infected_handler.h"

void respondToKeyPress(World& world, Player& player, int key);
void drawMapLimitBorders(World world);
void drawGameStatus(World world, Player player);
void drawInfected(World& world);
void drawWorldItems(World& world);
void drawRescueCountdown(World world);
void drawRescue(World& world);
void displayEndGame(World world, Player player);

enum GameControls{
    QUIT_KEY = KEY_BACKSPACE,
    FORWARD_KEY = static_cast<int>('w'),
    BACKWARD_KEY = static_cast<int>('s'),
    LEFT_KEY = static_cast<int>('a'),
    RIGHT_KEY = static_cast<int>('d'),
    SHOOT_KEY = static_cast<int>(' '),
    RELOAD_KEY = static_cast<int>('r'),
    FAST_RELOAD_KEY = static_cast<int>('R'),
    GRENADE_KEY = static_cast<int>('g'),
    PICKUP_KEY = static_cast<int>('e'),
    SWITCH_WEAPON_KEY = static_cast<int>('q'),
    CLAYMORE_KEY = static_cast<int>('c'),
};

void playGame(World world)
{
    initializeAudio();
    Player myPlayer;
    myPlayer.inventory.magazines.push_back(Magazine(
            CARTRIDGE_TYPE::CARTRIDGE_22LR, 10, 10));
    myPlayer.activeWeapon.magazine.isHollowPoint = true;
    myPlayer.inventory.magazines.at(0).isHollowPoint = true;

    // Arrival times for rescue.
    world.rescue.timeAtArrival = world.startTime + 300;
    world.rescue.timeAtEscape = world.rescue.timeAtArrival + 15;


    // Add coordinates to player.
    myPlayer.coordinates = std::make_pair(
        (world.mapColumnLimits.second - world.mapColumnLimits.first) * 0.5 +
                world.mapColumnLimits.first,
        (world.mapRowLimits.second - world.mapRowLimits.first) * 0.75 +
                world.mapRowLimits.first
    );

    myPlayer.fixWeaponAppearance();

    auto clearMap = [&](){
        auto colLimits = world.mapColumnLimits;
        auto rowLimits = world.mapRowLimits;
        for (int row = rowLimits.first - 1; row <= rowLimits.second + 1; ++row)
            for (int col = colLimits.first - 1; col <= colLimits.second + 1; ++col){
                move(row, col);
                printw(" ");
            }
    };

    auto drawPlayer = [&](Player player){
        mvprintw(player.coordinates.second, player.coordinates.first, player.playerChar);
        mvprintw(player.weaponCoordinates.second, player.weaponCoordinates.first, player.weaponChar);
    };

    auto checkPlayerIsDead = [&](Player player){
        for (auto inf : world.infected)
            if (inf.coordinates == player.coordinates && inf.alive)
                return true;
        return false;
    };

    initscr();
    noecho();
    keypad(stdscr, 1);
    curs_set(0);
    use_default_colors();
    nodelay(stdscr, 1);
    erase();
    drawMapLimitBorders(world);

    int gameStartEpoch = std::time(0);
    while (world.active)
    {
        clearMap();
        int capturedKey = getch();

        respondToKeyPress(world, myPlayer, capturedKey);

        // 1% chance of spawning an infected during this loop.
        if (std::rand() % 101 == 100){
            Infected inf{.coordinates=getInfectedSpawnPosition(world)};
            world.infected.push_back(inf);
        }

        // Drop supplies at intervals.
        if (world.nextSupplyDropEpoch <= std::time(0))
            world.dropSupplies();

        if (checkPlayerIsDead(myPlayer))
            world.active = false;

        // Update infected positions closer to the player at intervals.
        if (world.latestInfectedPositionUpdate + world.infectedPositionUpdateMs <= getEpochMs()){
            world.latestInfectedPositionUpdate = getEpochMs();
            updateInfectedPositions(world, myPlayer);
        }

        // Display stuff.
        removeDeadInfected(world);
        drawWorldItems(world);
        drawInfected(world);
        drawPlayer(myPlayer);
        drawGameStatus(world, myPlayer);
        drawRescueCountdown(world);

        // Draw rescue.
        if (!world.rescue.hasArrived && world.rescue.timeAtArrival <= std::time(0))
            world.rescue.triggerRescueArrival(world);
        else if (world.rescue.hasArrived && !world.rescue.isRescueFinished){
            drawRescue(world);
            bool coordinatesMatch = myPlayer.coordinates == world.rescue.coordinates;
            if (coordinatesMatch && world.rescue.canBoard){
                myPlayer.gameOverMessage = "You have been rescued!";
                world.active = false;
            }
            world.rescue.isRescueFinished = world.rescue.timeAtEscape <= std::time(0);
        }
        else if (world.rescue.isRescueFinished && !myPlayer.isRescued){
            myPlayer.gameOverMessage = "Failed to get rescued in time!";
            world.active = false;
        }

        refresh();
        napms(25);

        // End game if player is not alive.
        if (!myPlayer.alive)
            world.active = false;
    }

    std::thread(cleanupAudio).detach();
    displayEndGame(world, myPlayer);

    bool hasQuit = false;
    while (!hasQuit){
        auto key = getch();
        if (key == QUIT_KEY)
            hasQuit = true;
        napms(25);
    }

    clear();
    refresh();
    endwin();
}

void respondToKeyPress (World& world, Player& player, int key)
{
    auto fixPlayerDirection = [&](DIRECTION direction)
    {
        if (player.facingDirection != direction)
            player.facingDirection = direction;
    };

    switch (key){
        case QUIT_KEY:
            world.active = false;
            break;
        case FORWARD_KEY:
            if (player.coordinates.second - 1 >= world.mapRowLimits.first){
                player.coordinates.second -= 1;
                fixPlayerDirection(NORTH);
                player.fixWeaponAppearance();
            }
            break;
        case BACKWARD_KEY:
            if (player.coordinates.second + 1 <= world.mapRowLimits.second){
                player.coordinates.second += 1;
                fixPlayerDirection(SOUTH);
                player.fixWeaponAppearance();
            }
            break;
        case LEFT_KEY:
            if (player.coordinates.first - 1 >= world.mapColumnLimits.first){
                player.coordinates.first -= 1;
                fixPlayerDirection(WEST);
                player.fixWeaponAppearance();
            }
            break;
        case RIGHT_KEY:
            if (player.coordinates.first + 1 <= world.mapColumnLimits.second){
                player.coordinates.first += 1;
                fixPlayerDirection(EAST);
                player.fixWeaponAppearance();
            }
            break;
        case SHOOT_KEY:{
            std::thread(&Player::shootFirearm, &player).detach();
            handleFirearmShot(world, player);

            // Spawn new infected if they hear a gunshot.
            if (player.activeWeapon.cartridgeType !=
                    CARTRIDGE_TYPE::CARTRIDGE_22LR && std::rand() % 26 == 25)
            {
                Infected inf{.coordinates=getInfectedSpawnPosition(world)};
                world.infected.push_back(inf);
            }
            break;
        }
        case RELOAD_KEY:{
            std::thread reloadThread(&Player::reloadFirearm, &player);
            reloadThread.detach();
            break;
        }
        case FAST_RELOAD_KEY:{
            std::thread reloadThread(&Player::fastReloadFirearm, &player);
            reloadThread.detach();
            break;
        }
        case GRENADE_KEY:{
            auto grenadeOptional = player.throwGrenade(world);
            if (!grenadeOptional.has_value())
                break;

            Explosive grenade = grenadeOptional.value();
            world.activeExplosives.push_back(grenade);

            std::thread(processGrenadeThrow, std::ref(world), grenade).detach();
            std::thread(handleGrenadeExplosion, std::ref(world), std::ref(player), grenade._explosiveId.value()).detach();
            break;
        }
        case CLAYMORE_KEY:{
            if (player.hasPlantedClaymore){
                Explosive claymore(EXPLOSIVE_TYPE::__COUNT);
                for (auto cl : world.activeExplosives){
                    if (cl.explosiveType == EXPLOSIVE_TYPE::M18A1_CLAYMORE){
                        claymore = cl;
                        break;
                    }
                }
                std::thread(handleClaymoreExplosion, std::ref(world), std::ref(player), claymore).detach();
            }
            else
                player.plantClaymore(world);
            break;
        }
        case PICKUP_KEY:{
            player.pickupItem(world);
            break;
        }
        case SWITCH_WEAPON_KEY:{
            player.switchFirearm();
            break;
        }
    }
};

void drawMapLimitBorders(World world)
{
    auto colLimits = world.mapColumnLimits;
    auto rowLimits = world.mapRowLimits;
    std::string vertBorder((colLimits.second + 2) - (colLimits.first - 2), '#');
    mvprintw(rowLimits.first - 2, colLimits.first - 2, vertBorder.c_str());
    mvprintw(rowLimits.second + 2, colLimits.first - 2, vertBorder.c_str());
    for (int row = rowLimits.first - 2; row <= rowLimits.second + 2; ++row){
        mvprintw(row, colLimits.first - 2, "#");
        mvprintw(row, colLimits.second + 2, "#");
    }
}

void drawGameStatus(World world, Player player)
{
    for (int i = 0; i < 4; ++i){
        move(i, 0);
        clrtoeol();
    }
    player.updateHudText();
    mvprintw(0, 0, player.hudText.c_str());
}

void drawRescueCountdown(World world)
{
    Rescue rescue = world.rescue;
    bool hasArrived = rescue.hasArrived;
    time_t countdownEpoch = hasArrived ? rescue.timeAtEscape : rescue.timeAtArrival;

    std::string txt = hasArrived ? "Escape: " : "Rescue Arrival: ";
    txt += makeClockString(countdownEpoch - std::time(0));

    int columnBorderLength = world.mapColumnLimits.second - world.mapColumnLimits.first;
    std::pair<int, int> labelPos = std::make_pair(
            columnBorderLength/2 - txt.length()/2,
            world.mapRowLimits.first - 3);

    move(labelPos.second, labelPos.first);
    clrtoeol();
    mvprintw(labelPos.second, labelPos.first, txt.c_str());
}

void drawRescue(World& world)
{
    Rescue rescue = world.rescue;
    std::pair<int, int> rescueCoordinates = rescue.coordinates;
    mvprintw(rescueCoordinates.second, rescueCoordinates.first, rescue.rescueChar);
}

void drawInfected(World& world)
{
    for (auto inf : world.infected)
        if (inf.deathSplatter.has_value())
            for (auto coord : inf.deathSplatter->coordinates)
                mvprintw(coord.second, coord.first,
                        inf.deathSplatter->splatterChar);
    for (auto inf : world.infected)
        if (!inf.alive)
            mvprintw(inf.coordinates.second, inf.coordinates.first,
                    inf.infectedChar);
    for (auto inf : world.infected)
        if (inf.alive)
            mvprintw(inf.coordinates.second, inf.coordinates.first,
                    inf.infectedChar);
}

void drawWorldItems(World& world)
{
    for (auto drop : world.supplyDrops)
        mvprintw(drop.coordinates.second, drop.coordinates.first, drop.itemChar);

    for (auto explosive : world.activeExplosives)
        mvprintw(explosive.coordinates.second, explosive.coordinates.first, explosive.explosiveChar);
}

void displayEndGame(World world, Player player)
{
    clear();
    refresh();

    int playtime = std::time(0) - static_cast<int>(world.startTime);
    std::stringstream endGameMsg;
    if (!player.gameOverMessage.empty())
        endGameMsg << player.gameOverMessage << "\n\n";
    endGameMsg << "kills: " << player.killCount << "\n";
    endGameMsg << "playtime: " << makeClockString(playtime);

    mvprintw(0, 0, endGameMsg.str().c_str());
    refresh();
}
