#include <ncurses.h>
#include <string>
#include <utility>
#include <sstream>
#include "graphics/text_graphics.h"
#include "entities/rescue.h"
#include "string_utils.h"
#include "time_utils.h"

void drawGameStatus(World& world, Player& player)
{
    // Clears the rows above the map border -- game status area.
    for (int i = 0; i < 4; ++i){
        move(i, 0);
        clrtoeol();
    }
    player.updateHudText();
    mvprintw(0, 0, player.hudText.c_str());
}

void drawRescueCountdown(World& world)
{
    Rescue rescue = world.rescue;
    bool hasArrived = rescue.hasArrived;

    double countdownEpoch = hasArrived ? rescue.escapeEpoch :
            rescue.arrivalEpoch;

    std::string txt = hasArrived ? "Escape: " : "Rescue Arrival: ";
    int secondsTimer = countdownEpoch - getEpochAsDecimal();
    txt += makeClockString(secondsTimer);

    int columnBorderLength = world.mapColumnLimits.second -
            world.mapColumnLimits.first;

    std::pair<int, int> labelPos = std::make_pair(
            columnBorderLength/2 - txt.length()/2,
            world.mapRowLimits.first - 3);

    move(labelPos.second, labelPos.first);
    clrtoeol();
    mvprintw(labelPos.second, labelPos.first, txt.c_str());
}

void displayEndGame(World& world, Player& player)
{
    clear();
    refresh();

    int playtime = getEpochAsDecimal() - static_cast<int>(world.startTime);

    std::stringstream endGameMsg;

    if (player.gameStats.endGameMessage != ""){
        endGameMsg << player.gameStats.endGameMessage << "\n\n";
    }

    auto checkAndAdd = [&](std::string msg, int count){
        if (count > 0)
            endGameMsg << msg << count << "\n";
    };

    checkAndAdd("kills: ", player.gameStats.kills);
    checkAndAdd("headshot kills: ", player.gameStats.headshots);
    checkAndAdd("grenade kills: ", player.gameStats.grenadeKills);
    checkAndAdd("claymore kills: ", player.gameStats.claymoreKills);

    endGameMsg << "playtime: " << makeClockString(playtime);

    mvprintw(0, 0, endGameMsg.str().c_str());
    refresh();
}
