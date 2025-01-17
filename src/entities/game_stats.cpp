#include "entities/game_stats.h"

void GameStats::addKill()
{
    ++kills;
}

void GameStats::addHeadshot()
{
    ++headshots;
}

void GameStats::setEndGameMessage(std::string msg)
{
    this->endGameMessage = msg;
}

void GameStats::addGrenadeKill()
{
    ++this->grenadeKills;
}

void GameStats::addClaymoreKill()
{
    ++this->claymoreKills;
}

void GameStats::addM16MineKill()
{
    ++this->m16MineKills;
}
