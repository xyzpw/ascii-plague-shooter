#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <algorithm>
#include <unordered_map>
#include <optional>
#include "common.h"
#include "math_utils.h"
#include "gameUtilities.h"
#include "physics_manager.h"

bool checkHasMag(Inventory inventory, CARTRIDGE_TYPE cartridge);
std::unordered_map<EXPLOSIVE_TYPE, int> checkExplosiveInventorySize(Inventory inventory);
int checkMagCount(Inventory inventory, CARTRIDGE_TYPE cartridge);
bool checkHasFirearm(Inventory inventory, FIREARM_TYPE type);

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
        if (charCount == 0 && activeWeapon.loadedRounds > 0)
            charCount = 1;
        std::string str = std::string("[") + std::string(charCount, '|');
        str += std::string(maxLength - charCount, ' ') + "]";
        if (mag.isHollowPoint)
            str += " HP";
        return str;
    };

    auto makeMagString = [&](){
        std::string str = "";
        for (auto mag : inventory.magazines)
        {
            if (mag.cartridgeType == activeWeapon.magazine.cartridgeType)
            {
                double carCount = mag.cartridgeCount;
                double carCapacity = mag.capacity;
                double magDecimal = static_cast<double>(carCount) / static_cast<double>(carCapacity);
                for (auto frac : magazineAscii)
                    if (magDecimal >= frac.first){
                        str += frac.second + " ";
                        break;
                    }
            }
        }
        return str;
    };
    bool displayMags = checkHasMag(inventory, activeWeapon.magazine.cartridgeType);
    bool displayNoAmmo = !displayMags && activeWeapon.loadedRounds < 1;
    bool displayRounds = activeWeapon.loadedRounds >= 1;

    hudText = std::string("Active firearm: ") + activeWeapon.name;

    if (isReloading)
        hudText += " (reloading...)";

    if (displayNoAmmo)
        hudText += " (no ammo)";
    else
        hudText += " " + makeBulletsString();

    if (displayMags)
        hudText += " / " + makeMagString();

    auto explosiveInvSize = checkExplosiveInventorySize(inventory);
    int m67Count =
            explosiveInvSize.count(EXPLOSIVE_TYPE::M67_GRENADE) ?
            explosiveInvSize.at(EXPLOSIVE_TYPE::M67_GRENADE) : 0;

    int claymoreCount = explosiveInvSize.count(EXPLOSIVE_TYPE::M18A1_CLAYMORE) ?
            explosiveInvSize.at(EXPLOSIVE_TYPE::M18A1_CLAYMORE) : 0;

    if (m67Count)
        hudText += "\nm67 grenades: " + std::to_string(m67Count);
    if (claymoreCount){
        hudText += m67Count > 0 ? ", " : "\n";
        hudText += "claymores: " + std::to_string(claymoreCount);
    }
    hudText += "\nkills: " + std::to_string(killCount);
}

void Player::fixWeaponAppearance()
{
    bool isVert = facingDirection == NORTH || facingDirection == SOUTH;
    weaponChar = isVert ? "|" : "\u2015";
    if (isVert){
        int rowIncrease = facingDirection == NORTH ? -1 : 1;
        weaponCoordinates = std::make_pair(coordinates.first, coordinates.second + rowIncrease);
    }
    else{
        int colIncrease = facingDirection == WEST ? -1 : 1;
        weaponCoordinates = std::make_pair(coordinates.first + colIncrease, coordinates.second);
    }
}

void Player::shootFirearm()
{
    if (!activeWeapon.canShoot || activeWeapon.isReloading)
        return;
    canSwitchFirearm = false;
    activeWeapon.canShoot = false;

    std::thread(playAudio, activeWeapon.shootAudioFile).detach();

    activeWeapon.magazine.cartridgeCount -= 1;
    activeWeapon.loadedRounds -= 1;
    if (activeWeapon.isChambered && activeWeapon.loadedRounds == 0){
        activeWeapon.isChambered = false;
        activeWeapon.magazine.cartridgeCount = 0;
    }

    usleep(activeWeapon.shootIntervalMs * 1e3);
    canSwitchFirearm = true;
    activeWeapon.canShoot = activeWeapon.loadedRounds >= 1;
}

void Player::reloadFirearm()
{
    bool hasMag = checkHasMag(inventory, activeWeapon.magazine.cartridgeType);
    if (isReloading || !hasMag)
        return;

    activeWeapon.canShoot = false;
    isReloading = true;

    Magazine currentMag = activeWeapon.magazine;

    activeWeapon.loadedRounds = activeWeapon.isChambered ? 1 : 0;
    if (currentMag.cartridgeCount == 0){
        isReloading = false;
        fastReloadFirearm();
        return;
    }

    usleep(activeWeapon.reloadTime * 1e6);

    std::sort(inventory.magazines.begin(), inventory.magazines.end(), [&](Magazine a, Magazine b){
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
    std::sort(inventory.magazines.begin(), inventory.magazines.end(), [&](Magazine a, Magazine b){
        return a.cartridgeCount > b.cartridgeCount;
    });
    activeWeapon.canShoot = true;
    isReloading = false;
    activeWeapon.loadedRounds += activeWeapon.magazine.cartridgeCount;
}

void Player::fastReloadFirearm()
{
    bool hasMag = checkHasMag(inventory, activeWeapon.magazine.cartridgeType);
    if (isReloading || !hasMag)
        return;

    activeWeapon.canShoot = false;
    isReloading = true;

    Magazine& currentMag = activeWeapon.magazine;
    activeWeapon.loadedRounds = activeWeapon.isChambered ? 1 : 0;

    usleep(activeWeapon.fastReloadTime * 1e6);


    std::sort(inventory.magazines.begin(), inventory.magazines.end(), [&](Magazine a, Magazine b){
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
    if (isReloading)
        return std::nullopt;

    // Remove grenade from inventory and throw it.
    std::optional<Explosive> grenade;
    for (auto explosive = inventory.explosives.begin(); explosive != inventory.explosives.end(); ++explosive)
    {
        if (explosive->explosiveType == EXPLOSIVE_TYPE::M67_GRENADE)
        {
            grenade = *explosive;
            grenade->_explosiveId = randInt();
            grenade->facingDirection = facingDirection;
            grenade->coordinates = coordinates;

            inventory.explosives.erase(explosive); // Throw grenade
            break;
        }
    }

    if (!grenade.has_value())
        return std::nullopt;

    return grenade;
}


void Player::plantClaymore(World& world)
{
    bool hasClaymore = checkExplosiveInventorySize(inventory).count(EXPLOSIVE_TYPE::M18A1_CLAYMORE);
    if (isReloading || !hasClaymore)
        return;
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
        int newRow = std::clamp(rowIncrease + coordinates.second, world.mapRowLimits.first, world.mapRowLimits.second);
        claymore.coordinates = std::make_pair(coordinates.first, newRow);
    }
    else{
        claymore.explosiveChar = facingDirection == EAST ? "<" : ">";
        int colIncrease = facingDirection == EAST ? +1 : -1;
        int newCol = std::clamp(colIncrease + coordinates.first, world.mapColumnLimits.first, world.mapColumnLimits.second);
        claymore.coordinates = std::make_pair(newCol, coordinates.second);
    }

    world.activeExplosives.push_back(claymore);
    hasPlantedClaymore = true;
}

void Player::pickupItem(World& world)
{
    if (isReloading)
        return;

    for (auto drop = world.supplyDrops.begin(); drop != world.supplyDrops.end(); ++drop)
    {
        if (getMapPointsDistance(coordinates, drop->coordinates) > 2)
            continue;

        for (auto item : drop->items.firearms)
        {
            if (checkHasFirearm(inventory, item.firearmType) ||
                    activeWeapon.firearmType == item.firearmType)
            {
                inventory.magazines.push_back(item.magazine);
                continue;
            }
            inventory.firearms.push_back(item);
            if (!activeWeapon.loadedRounds && !checkHasMag(inventory, activeWeapon.magazine.cartridgeType))
                switchFirearm();
        }
        for (auto item : drop->items.explosives)
            inventory.explosives.push_back(item);
        for (auto item : drop->items.magazines)
            inventory.magazines.push_back(item);
        world.supplyDrops.erase(drop);
        break;
    }
}

void Player::switchFirearm()
{
    if (inventory.firearms.size() == 0 || activeWeapon.isReloading || !canSwitchFirearm)
        return;
    FIREARM_TYPE currentType = activeWeapon.firearmType;
    inventory.firearms.push_back(activeWeapon);
    for (auto it = inventory.firearms.begin(); it != inventory.firearms.end();)
    {
        if (it->firearmType != currentType){
            activeWeapon = *it;
            inventory.firearms.erase(it);
            break;
        }
        else
            ++it;
    }

    // Remove firearms from inventory if no ammo exists.
    for (auto firearm = inventory.firearms.begin(); firearm != inventory.firearms.end();)
    {
        if (firearm->loadedRounds == 0 && !checkHasMag(inventory, firearm->magazine.cartridgeType)){
            inventory.firearms.erase(firearm);
            break;
        }
        else
            ++firearm;
    }
}

bool checkHasMag(Inventory inventory, CARTRIDGE_TYPE cartridge)
{
    for (auto mag : inventory.magazines)
    {
        if (mag.cartridgeType == cartridge)
            return true;
    }
    return false;
}

std::unordered_map<EXPLOSIVE_TYPE, int> checkExplosiveInventorySize(Inventory inventory)
{
    std::unordered_map<EXPLOSIVE_TYPE, int> explosives{};
    std::vector<EXPLOSIVE_TYPE> invExplosive{};

    auto appendToMap = [&](EXPLOSIVE_TYPE type){
        if (std::find(invExplosive.begin(), invExplosive.end(), type) == invExplosive.end()){
            explosives[type] = 1;
            invExplosive.push_back(type);
        }
        else
            explosives[type] += 1;
    };

    for (auto ex : inventory.explosives)
    {
        appendToMap(ex.explosiveType);
    }
    return explosives;
}

int checkMagCount(Inventory inventory, CARTRIDGE_TYPE cartridge)
{
    int count = 0;
    for (auto cart : inventory.magazines)
        if (cart.cartridgeType == cartridge)
            ++count;
    return count;
};

bool checkHasFirearm(Inventory inventory, FIREARM_TYPE type)
{
    for (auto firearm : inventory.firearms)
        if (firearm.firearmType == type)
            return true;
    return 0;
}
