#pragma once

#include <unordered_map>

struct GameSettings{
    bool colors;
    bool quickGame;
    void setSettingsFromArgs(
        std::unordered_map<std::string, std::string> args
    );
};
