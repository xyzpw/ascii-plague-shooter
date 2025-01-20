#include <unordered_map>
#include <string>
#include "game_settings.h"

void GameSettings::setSettingsFromArgs(
    std::unordered_map<std::string, std::string> args)
{
    if (args.count("no-color")){
        this->colors = false;
    }
    if (args.count("quick")){
        this->quickGame = true;
    }
    if (args.count("no-animation")){
        this->animations = false;
    }
}
