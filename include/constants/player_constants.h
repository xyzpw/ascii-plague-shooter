#pragma once

constexpr int PLAYER_THROW_VELOCITY_MIN = 15;
constexpr int PLAYER_THROW_VELOCITY_MAX = 20;

constexpr int PLAYER_THROW_ANGLE_DEGREES_MIN = 40;
constexpr int PLAYER_THROW_ANGLE_DEGREES_MAX = 50;

constexpr const char* PLAYER_CHAR = "O";
constexpr const char* PLAYER_WEAPON_CHAR_VERTICAL = "|";
constexpr const char* PLAYER_WEAPON_CHAR_HORIZONTAL = "\u2015";

constexpr const char* GAME_END_MSG_GRENADE = "cause of death: grenade";
constexpr const char* GAME_END_MSG_CLAYMORE = "cause of death: claymore";
constexpr const char* GAME_END_MSG_RESCUED = "You have been rescued!";
constexpr const char* GAME_END_MSG_RESCUE_FAILED = "You have been left behind!";
