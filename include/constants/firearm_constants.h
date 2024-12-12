#pragma once

// Fraction of energy a bullet retains after penetrating an enemy.
constexpr double BULLET_PENETRATE_KE_FACTOR = 0.8;
constexpr double BULLET_PENETRATE_KE_FACTOR_HP = 0.1;

constexpr int BULLET_KE_9MM = 536;
constexpr int BULLET_KE_223_REMINGTON = 1738;
constexpr int BULLET_KE_223_REMINGTON_HP = 1300;
constexpr int BULLET_KE_30_06 = 3894;
constexpr int BULLET_KE_22LR = 138;
constexpr double BULLET_KE_LOSS_9MM = 1.4;
constexpr double BULLET_KE_LOSS_223_REMINGTON = 4.4;
constexpr double BULLET_KE_LOSS_30_06 = 6.32;
constexpr double BULLET_KE_LOSS_22LR = 0.63;

constexpr double HANDGUN_ACCURACY_DECAY = 0.054;
constexpr double HANDGUN_ACCURACY_MULTIPLIER = 1.74;
constexpr double RIFLE_ACCURACY_DECAY = 0.0135;
constexpr double RIFLE_ACCURACY_MULTIPLIER = 1.7;
constexpr double A_RIFLE_ACCURACY_DECAY = 0.02;
constexpr double A_RIFLE_ACCURACY_MULTIPLIER = 1.69;

// Cost of each cartridge -- supply drop delay in seconds.
constexpr double CARTRIDGE_9MM_COST = 0.96;
constexpr double CARTRIDGE_9MM_HP_COST = 1.7;
constexpr double CARTRIDGE_223_REMINGTON_COST = 1.4;
constexpr double CARTRIDGE_223_REMINGTON_HP_COST = 2;
constexpr double CARTRIDGE_30_06_COST = 3.9;
