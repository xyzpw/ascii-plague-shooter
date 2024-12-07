#pragma once

#include <string>
#include <optional>

#include "entities/firearm.h"
#include "entities/inventory.h"
#include "entities/explosive.h"
#include "entities/game_stats.h"
#include "constants/player_constants.h"
#include "world_enums.h"
#include "weapon_enums.h"
#include "world.h"
#include "position.h"

struct Player{
    const char* playerChar = PLAYER_CHAR;
    const char* weaponChar = PLAYER_WEAPON_CHAR_VERTICAL;
    std::string hudText = "";
    bool alive = true;
    bool canSwitchFirearm = true;
    Firearm activeWeapon;
    Inventory inventory;

    Position position;
    Position weaponPosition;

    DIRECTION facingDirection = NORTH;
    GameStats gameStats;
    bool isReloading = false;
    bool hasPlantedClaymore = false;
    bool isRescued = false;
    void fixWeaponAppearance();
    void shootFirearm();
    std::optional<Explosive> throwGrenade(World& world);
    void updateHudText();
    void pickupItem(World& world);
    void switchFirearm();
    void reloadFirearm();
    void fastReloadFirearm();
    void plantClaymore(World& world);
    Player() : activeWeapon(FIREARM_TYPE::RUGER_MK_IV){};
};