#pragma once

#include <string>

struct GameStats{
    std::string endGameMessage = "";
    int kills = 0;
    int headshots = 0;
    int grenadeKills = 0;
    int claymoreKills = 0;
    int m16MineKills = 0;
    void addKill();
    void addHeadshot();
    void addGrenadeKill();
    void addClaymoreKill();
    void addM16MineKill();
    void setEndGameMessage(std::string);
};
