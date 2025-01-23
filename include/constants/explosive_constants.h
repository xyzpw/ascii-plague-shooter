#pragma once

constexpr int CLAYMORE_FRAGMENT_DEGREES = 60;

constexpr int M67_PASCALS = 1025422;
constexpr int CLAYMORE_PASCALS = 3814639;
constexpr int M16_MINE_PASCALS = 1'897'830;

constexpr int M67_FRAGMENT_COUNT_MIN = 774;
constexpr int M67_FRAGMENT_COUNT_MAX = 3483;
constexpr int CLAYMORE_FRAGMENT_COUNT = 700;
constexpr int M16_MINE_FRAGMENT_COUNT = 923;

constexpr int M67_FRAGMENT_KE_SCALE_NUMERATOR = 391'894;
constexpr int CLAYMORE_FRAGMENT_KE = 100;
constexpr int M16_MINE_FRAGMENT_KE = 133;

constexpr double M67_FRAGMENT_KE_LOSS = 2.0;
constexpr double CLAYMORE_FRAGMENT_KE_LOSS = 3.4;
constexpr double M16_MINE_FRAGMENT_KE_LOSS = 3.1;

constexpr double M67_PENETRATE_ENERGY_THRESHOLD = 2.5;
constexpr double CLAYMORE_PENETRATE_ENERGY_THRESHOLD = 1.6;
constexpr double M16_MINE_PENETRATE_ENERGY_THRESHOLD = 6.9;

constexpr double M67_EXPLOSION_DELAY_MIN = 4.0;
constexpr double M67_EXPLOSION_DELAY_MAX = 5.0;
constexpr double CLAYMORE_EXPLOSION_DELAY = 1.0;
constexpr double M16_MINE_EXPLOSION_DELAY = 0.5;

constexpr const char* GRENADE_CHAR = "\u2022";
constexpr const char* CLAYMORE_CHAR_DEFAULT = "<";
constexpr const char* M16_MINE_CHAR = "*";

