#include <unordered_map>
#include <string>
#include <unistd.h>
#include <algorithm>
#include <optional>
#include <thread>
#include "entities/player.h"
#include "entities/inventory.h"
#include "entities/magazine.h"
#include "entities/explosive.h"
#include "weapon_enums.h"
#include "inventory_utils.h"
#include "world.h"
#include "game_utils.h"
#include "audio_handler.h"
#include "random_utils.h"
#include "math_utils.h"

bool checkHasMag(Inventory inventory, CARTRIDGE_TYPE cartridge);
bool checkHasAmmunition(Inventory&, CARTRIDGE_TYPE);

std::unordered_map<double, std::string> magazineAscii{
    {0.125, "\u2581"},
    {0.25, "\u2582"},
    {0.375, "\u2583"},
    {0.5, "\u2584"},
    {0.625, "\u2585"},
    {0.75, "\u2586"},
    {0.875, "\u2589"},
    {1.0, "\u2588"},
};

void Player::updateHudText()
{
    auto makeBulletsString = [&](){
        int maxLength = 10;

        Magazine& mag = activeWeapon.magazine;

        double bulletFrac = mag.cartridgeCount / static_cast<double>(mag.capacity);
        int charCount = bulletFrac * maxLength;
        if (charCount == 0 && activeWeapon.loadedRounds > 0){
            charCount = 1;
        }
        std::string str = std::string("[") + std::string(charCount, '|');
        str += std::string(maxLength - charCount, ' ') + "]";
        if (mag.isHollowPoint){
            str += " HP";
        }
        return str;
    };

    auto makeMagString = [&](){
        std::string str = "";
        if (this->activeWeapon.feedSystem == RELOAD_TYPE::DIRECT_LOAD){
            str = std::string(
                getInventoryAmmunitionCount(
                    inventory,
                    activeWeapon.cartridgeType
                ), '|'
            );
            return str;
        }
        for (auto mag : inventory.magazines)
        {
            if (mag.cartridgeType == activeWeapon.cartridgeType)
            {
                double carCount = mag.cartridgeCount;
                double carCapacity = mag.capacity;
                double magDecimal = static_cast<double>(carCount) /
                                    static_cast<double>(carCapacity);
                for (auto frac : magazineAscii){
                    if (magDecimal >= frac.first){
                        str += frac.second + " ";
                        break;
                    }
                    else if (magDecimal <= 0.125){
                        str += "\u2581 ";
                        break;
                    }
                }
            }
        }
        return str;
    };

    bool displayMags = checkHasMag(inventory, activeWeapon.cartridgeType);
    if (activeWeapon.feedSystem == RELOAD_TYPE::DIRECT_LOAD){
        displayMags = checkHasAmmunition(inventory, activeWeapon.cartridgeType);
    }

    bool displayNoAmmo = !displayMags && activeWeapon.loadedRounds < 1;
    bool displayRounds = activeWeapon.loadedRounds >= 1;

    hudText = std::string("Active firearm: ") + activeWeapon.name;

    if (isReloading){
        hudText += " (reloading...)";
    }

    if (displayNoAmmo){
        hudText += " (no ammo)";
    }
    else{
        hudText += " " + makeBulletsString();
    }

    if (displayMags){
        hudText += " / " + makeMagString();
    }

    auto explosiveInvSize = getInventoryExplosiveSize(inventory);
    int m67Count =
            explosiveInvSize.count(EXPLOSIVE_TYPE::M67_GRENADE) ?
            explosiveInvSize.at(EXPLOSIVE_TYPE::M67_GRENADE) : 0;

    int claymoreCount = explosiveInvSize.count(EXPLOSIVE_TYPE::M18A1_CLAYMORE) ?
            explosiveInvSize.at(EXPLOSIVE_TYPE::M18A1_CLAYMORE) : 0;

    if (m67Count){
        hudText += "\nm67 grenades: " + std::to_string(m67Count);
    }
    if (claymoreCount){
        hudText += m67Count > 0 ? ", " : "\n";
        hudText += "claymores: " + std::to_string(claymoreCount);
    }
    hudText += "\nkills: " + std::to_string(gameStats.kills);
}

void Player::fixWeaponAppearance()
{
    bool isVert = facingDirection == NORTH || facingDirection == SOUTH;
    weaponChar = isVert ? PLAYER_WEAPON_CHAR_VERTICAL :
                        PLAYER_WEAPON_CHAR_HORIZONTAL;

    if (isVert){
        int rowIncrease = facingDirection == NORTH ? -1 : 1;
        this->weaponPosition.column = position.column;
        this->weaponPosition.row = position.row + rowIncrease;
    }
    else {
        int colIncrease = facingDirection == WEST ? -1 : 1;
        this->weaponPosition.column = position.column + colIncrease;
        this->weaponPosition.row = position.row;
    }
}

void Player::shootFirearm()
{
    if (!activeWeapon.canShoot || isReloading){
        return;
    }
    canSwitchFirearm = false;
    activeWeapon.canShoot = false;

    std::thread(playAudio, activeWeapon.shootAudioFile).detach();

    activeWeapon.magazine.cartridgeCount -= 1;
    if (activeWeapon.magazine.cartridgeCount < 0){
        activeWeapon.magazine.cartridgeCount = 0;
    }

    activeWeapon.loadedRounds -= 1;
    if (activeWeapon.isChambered && activeWeapon.loadedRounds <= 0){
        activeWeapon.isChambered = false;
    }

    usleep(activeWeapon.shootIntervalMs * 1e3);
    canSwitchFirearm = true;
    activeWeapon.canShoot = activeWeapon.loadedRounds >= 1;
}

void Player::reloadFirearm()
{
    CARTRIDGE_TYPE cartridgeType = this->activeWeapon.cartridgeType;
    bool hasMag = checkHasMag(this->inventory, cartridgeType);
    bool hasCartridges = checkHasAmmunition(inventory, cartridgeType);
    bool isMagDetachable = activeWeapon.feedSystem ==
                            RELOAD_TYPE::DETACHABLE_MAGAZINE;

    if (isMagDetachable && (isReloading || !hasMag) ||
        !isMagDetachable && !hasCartridges)
    {
        return;
    }

    activeWeapon.canShoot = false;
    isReloading = true;

    Magazine currentMag = activeWeapon.magazine;

    if (isMagDetachable){
        activeWeapon.loadedRounds = activeWeapon.isChambered ? 1 : 0;
    }

    // Fast reload if mag is detachable and empty.
    if (isMagDetachable && currentMag.cartridgeCount == 0){
        isReloading = false;
        fastReloadFirearm();
        return;
    }

    // Add rounds to internal magazine if magazine is not detachable.
    if (!isMagDetachable){
        usleep(activeWeapon.chamberReloadDelay / 2.0 * 1e6);

        int rounds = activeWeapon.magazine.cartridgeCount;
        int capacity = activeWeapon.magazine.capacity;

        while (rounds < capacity &&
                checkHasAmmunition(inventory, cartridgeType))
        {
            usleep(activeWeapon.loadRoundTime.value() * 1e6);
            inventory.ammunition.at(cartridgeType) -= 1;
            ++rounds;
            activeWeapon.magazine.cartridgeCount += 1;
            activeWeapon.loadedRounds += 1;
        }

        usleep(activeWeapon.chamberReloadDelay / 2.0 * 1e6);

        isReloading = false;
        activeWeapon.canShoot = true;
        return;
    }

    usleep(activeWeapon.reloadTime * 1e6);

    std::sort(inventory.magazines.begin(), inventory.magazines.end(),
              [&](Magazine a, Magazine b)
    {
        return a.cartridgeCount > b.cartridgeCount;
    });
    for (auto mag = inventory.magazines.begin(); mag != inventory.magazines.end();)
    {
        if (mag->cartridgeType == currentMag.cartridgeType)
        {
            activeWeapon.magazine = *mag;
            inventory.magazines.erase(mag);
            break;
        }
        else
            ++mag;
    }

    inventory.magazines.push_back(currentMag);
    std::sort(inventory.magazines.begin(), inventory.magazines.end(),
              [&](Magazine a, Magazine b)
    {
        return a.cartridgeCount > b.cartridgeCount;
    });
    activeWeapon.canShoot = true;
    isReloading = false;
    activeWeapon.loadedRounds += activeWeapon.magazine.cartridgeCount;
}

void Player::fastReloadFirearm()
{
    CARTRIDGE_TYPE cartridgeType = activeWeapon.cartridgeType;
    bool hasMag = checkHasMag(inventory, activeWeapon.magazine.cartridgeType);
    bool hasAmmunition = checkHasAmmunition(inventory, cartridgeType);
    bool isMagDetachable = activeWeapon.feedSystem == RELOAD_TYPE::DETACHABLE_MAGAZINE;

    if (isMagDetachable && (isReloading || !hasMag) ||
        !isMagDetachable && !hasAmmunition)
    {
        return;
    }

    activeWeapon.canShoot = false;
    isReloading = true;

    Magazine& currentMag = activeWeapon.magazine;

    if (isMagDetachable){
        activeWeapon.loadedRounds = activeWeapon.isChambered ? 1 : 0;
    }

    // Add 1 round to internal magazine.
    if (!isMagDetachable){
        if (activeWeapon.loadedRounds >= activeWeapon.magazine.capacity){
            isReloading = false;
            activeWeapon.canShoot = true;
            return;
        }

        usleep(activeWeapon.chamberReloadDelay / 2.0 * 1e6);

        usleep(activeWeapon.loadRoundTime.value() * 1e6);
        inventory.ammunition.at(cartridgeType) -= 1;
        activeWeapon.magazine.cartridgeCount += 1;
        activeWeapon.loadedRounds += 1;

        usleep(activeWeapon.chamberReloadDelay / 2.0 * 1e6);

        isReloading = false;
        activeWeapon.canShoot = true;
        return;
    }

    usleep(activeWeapon.fastReloadTime * 1e6);


    std::sort(inventory.magazines.begin(), inventory.magazines.end(),
              [&](Magazine a, Magazine b)
    {
        return a.cartridgeCount > b.cartridgeCount;
    });

    for (auto mag = inventory.magazines.begin(); mag != inventory.magazines.end();)
    {
        if (mag->cartridgeType == currentMag.cartridgeType){
            activeWeapon.magazine = *mag;
            inventory.magazines.erase(mag);
            break;
        }
        else
            ++mag;
    }

    activeWeapon.loadedRounds += activeWeapon.magazine.cartridgeCount;
    if (!activeWeapon.isChambered){
        usleep(activeWeapon.chamberReloadDelay * 1e6);
        currentMag.cartridgeCount -= 1;
        activeWeapon.isChambered = true;
    }
    isReloading = false;
    activeWeapon.canShoot = true;
}

std::optional<Explosive> Player::throwGrenade(World& world)
{
    if (isReloading){
        return std::nullopt;
    }

    // Remove grenade from inventory and throw it.
    std::optional<Explosive> grenade;
    for (auto explosive = inventory.explosives.begin();
         explosive != inventory.explosives.end(); ++explosive)
    {
        if (explosive->explosiveType == EXPLOSIVE_TYPE::M67_GRENADE)
        {
            grenade = *explosive;
            grenade->_explosiveId = randInt();
            grenade->facingDirection = facingDirection;
            grenade->position = position;

            inventory.explosives.erase(explosive); // Throw grenade
            break;
        }
    }

    if (!grenade.has_value()){
        return std::nullopt;
    }

    return grenade;
}


void Player::plantClaymore(World& world)
{
    bool hasClaymore = getInventoryExplosiveSize(inventory).count(
        EXPLOSIVE_TYPE::M18A1_CLAYMORE
    );

    if (isReloading || !hasClaymore){
        return;
    }

    Explosive claymore(EXPLOSIVE_TYPE::M18A1_CLAYMORE);

    for (auto cl = inventory.explosives.begin(); cl != inventory.explosives.end();)
    {
        if (cl->explosiveType == EXPLOSIVE_TYPE::M18A1_CLAYMORE){
            inventory.explosives.erase(cl); // Plant claymore
            break;
        }
        else
            ++cl;
    }

    claymore._explosiveId = randInt();
    claymore.facingDirection = facingDirection;

    if (facingDirection == NORTH || facingDirection == SOUTH){
        claymore.explosiveChar = facingDirection == SOUTH ? "^" : "V";

        int rowIncrease = facingDirection == SOUTH ? +1 : -1;

        int newRow = std::clamp(
            rowIncrease + position.row,
            world.mapRowLimits.first,
            world.mapRowLimits.second
        );

        claymore.position.column = position.column;
        claymore.position.row = newRow;
    }
    else{
        claymore.explosiveChar = facingDirection == EAST ? "<" : ">";

        int colIncrease = facingDirection == EAST ? +1 : -1;

        int newCol = std::clamp(
            colIncrease + position.column,
            world.mapColumnLimits.first,
            world.mapColumnLimits.second
        );

        claymore.position.column = newCol;
        claymore.position.row = position.row;
    }

    world.activeExplosives.push_back(claymore);
    hasPlantedClaymore = true;
}

void Player::pickupItem(World& world)
{
    if (isReloading){
        return;
    }

    for (auto drop = world.supplyDrops.begin(); drop != world.supplyDrops.end();
         ++drop)
    {
        if (getPositionDistance(position, drop->position) > 2){
            continue;
        }

        for (auto item : drop->items.firearms)
        {
            // Only add ammo if the player already has the firearm.
            if (checkInventoryHasFirearm(inventory, item.firearmType) ||
                activeWeapon.firearmType == item.firearmType)
            {
                inventory.magazines.push_back(item.magazine);
                continue;
            }

            inventory.firearms.push_back(item);

            // Switch firearms if the player has no ammo for the active firearm.
            if (activeWeapon.loadedRounds == 0 &&
                !checkHasMag(inventory, activeWeapon.magazine.cartridgeType))
            {
                switchFirearm();
            }
            else if (activeWeapon.feedSystem == RELOAD_TYPE::DIRECT_LOAD &&
                     activeWeapon.loadedRounds == 0 &&
                     !checkHasAmmunition(inventory, activeWeapon.cartridgeType))
            {
                switchFirearm();
            }
        }

        for (auto item : drop->items.explosives){
            inventory.explosives.push_back(item);
        }
        for (auto item : drop->items.magazines){
            inventory.magazines.push_back(item);
        }

        for (auto item : drop->items.ammunition){
            if (inventory.ammunition.count(item.first)){
                inventory.ammunition[item.first] += item.second;
            }
            else{
                inventory.ammunition[item.first] = item.second;
            }
        }

        world.supplyDrops.erase(drop);
        break;
    }
}

void Player::switchFirearm()
{
    if (inventory.firearms.size() == 0 || isReloading ||
        !canSwitchFirearm)
    {
        return;
    }

    FIREARM_TYPE currentType = activeWeapon.firearmType;
    inventory.firearms.push_back(activeWeapon);

    // Find next firearm in player's inventory.
    auto it = std::find_if(inventory.firearms.begin(), inventory.firearms.end(),
                           [&](const Firearm& f)
    {
        return f.firearmType != currentType;
    });

    // Switch to found firearm if it exists in the player's inventory.
    if (it != inventory.firearms.end()){
        activeWeapon = *it;
        inventory.firearms.erase(it);
    }

    // Remove firearms from inventory if no ammo exists.
    for (auto firearm = inventory.firearms.begin();
         firearm != inventory.firearms.end();)
    {
        if (firearm->loadedRounds == 0 &&
            !checkHasMag(inventory, firearm->magazine.cartridgeType))
        {
            inventory.firearms.erase(firearm);
            break;
        }
        else{
            ++firearm;
        }
    }
}

/*
 Check if an inventory contains at least 1 magazine which holds
 a specified cartridge type.
*/
bool checkHasMag(Inventory inventory, CARTRIDGE_TYPE cartridge)
{
    for (auto mag : inventory.magazines)
    {
        if (mag.cartridgeType == cartridge){
            return true;
        }
    }
    return false;
}

// Check if an inventory contains at least 1 cartridge.
bool checkHasAmmunition(Inventory& inventory, CARTRIDGE_TYPE cartridge)
{
    int c = getInventoryAmmunitionCount(inventory, cartridge);
    return c >= 1;
}
