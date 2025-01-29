#pragma once

#include <ncurses.h>

enum GameControls{
    QUIT_KEY = KEY_BACKSPACE,
    FORWARD_KEY = static_cast<int>('w'),
    LOOK_FORWARD_KEY = static_cast<int>('W'),
    BACKWARD_KEY = static_cast<int>('s'),
    LOOK_BACKWARD_KEY = static_cast<int>('S'),
    LEFT_KEY = static_cast<int>('a'),
    LOOK_LEFT_KEY = static_cast<int>('A'),
    RIGHT_KEY = static_cast<int>('d'),
    LOOK_RIGHT_KEY = static_cast<int>('D'),
    SHOOT_KEY = static_cast<int>(' '),
    RELOAD_KEY = static_cast<int>('r'),
    FAST_RELOAD_KEY = static_cast<int>('R'),
    GRENADE_KEY = static_cast<int>('g'),
    GRENADE_KEY_CLOSE = static_cast<int>('G'),
    PICKUP_KEY = static_cast<int>('e'),
    SWITCH_WEAPON_KEY = static_cast<int>('q'),
    CLAYMORE_KEY = static_cast<int>('c'),
    M16_MINE_KEY = static_cast<int>('m'),
};
