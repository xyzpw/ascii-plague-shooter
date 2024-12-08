#include <unordered_map>
#include <string>
#include "game_settings.h"

void GameSettings::setSettingsFromArgs(
    std::unordered_map<std::string, std::string> args)
{
    if (args.count("colors")){
        this->colors = true;
    }
}
