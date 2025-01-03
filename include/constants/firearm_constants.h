#pragma once

#include <unordered_map>
#include "weapon_enums.h"

// Fraction of energy a bullet retains after penetrating an enemy.
constexpr double BULLET_PENETRATE_KE_FACTOR = 0.8;
constexpr double BULLET_PENETRATE_KE_FACTOR_HP = 0.1;

// Maximum number of enemies a bullet can perforate.
constexpr int MAX_PERFORATE_COUNT = 3;

// Kinetic energy of firearm projectiles.
constexpr int BULLET_KE_9MM = 536;
constexpr int BULLET_KE_223_REMINGTON = 1738;
constexpr int BULLET_KE_223_REMINGTON_HP = 1300;
constexpr int BULLET_KE_30_06 = 3894;
constexpr int BULLET_KE_22LR = 138;
constexpr int PELLET_KE_12GA_00_BUCKSHOT = 275;

// Kinetic energy loss for firearm projectiles per meter.
constexpr double BULLET_KE_LOSS_9MM = 1.5;
constexpr double BULLET_KE_LOSS_223_REMINGTON = 4.5;
constexpr double BULLET_KE_LOSS_30_06 = 6.32;
constexpr double BULLET_KE_LOSS_22LR = 0.49;
constexpr double PELLET_KE_LOSS_12GA_00_BUCKSHOT = 2.5;

constexpr int PELLET_COUNT_12GA_00_BUCKSHOT = 9;

// Spread of shotgun pellets --in degrees-- when fired.
constexpr double PELLET_SPREAD_SHOTGUN = 2.55;

inline const std::unordered_map<FIREARM_TYPE, double> FIREARM_ACCURACY_DECAYS = {
    {FIREARM_TYPE::SIG_M17, 0.054},
    {FIREARM_TYPE::AR15, 0.02},
    {FIREARM_TYPE::REMINGTON_700, 0.0135},
    {FIREARM_TYPE::RUGER_MK_IV, 0.054},
    {FIREARM_TYPE::BENELLI_M4, 0.0263},
};
inline const std::unordered_map<FIREARM_TYPE, double> FIREARM_ACCURACY_MULTIPLIERS = {
    {FIREARM_TYPE::SIG_M17, 1.74},
    {FIREARM_TYPE::AR15, 1.69},
    {FIREARM_TYPE::REMINGTON_700, 1.7},
    {FIREARM_TYPE::RUGER_MK_IV, 1.74},
    {FIREARM_TYPE::BENELLI_M4, 1.7},
};

// Cost of each cartridge -- supply drop delay in seconds.
constexpr double CARTRIDGE_9MM_COST = 0.96;
constexpr double CARTRIDGE_9MM_HP_COST = 1.7;
constexpr double CARTRIDGE_223_REMINGTON_COST = 2.8;
constexpr double CARTRIDGE_223_REMINGTON_HP_COST = 2.0;
constexpr double CARTRIDGE_30_06_COST = 3.9;
constexpr double CARTRIDGE_BUCKSHOT_COST = 1.5;
