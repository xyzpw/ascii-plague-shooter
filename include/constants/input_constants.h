#pragma once

#include <ncurses.h>

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