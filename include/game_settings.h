#pragma once

#include <unordered_map>

struct GameSettings{
    bool colors = true;
    bool quickGame;
    bool animations = true;
    void setSettingsFromArgs(
        std::unordered_map<std::string, std::string> args
    );
};
