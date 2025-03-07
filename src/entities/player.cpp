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
void handleInventoryPickupExistingFirearm(
    Inventory&, Firearm& activeWeapon, Firearm
);
bool checkIsShotgun(FIREARM_TYPE);

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
        if (activeWeapon.chamberRoundType == AMMO_TYPE::HOLLOW_POINT){
            str += " HP";
        }
        else if (activeWeapon.chamberRoundType == AMMO_TYPE::RIFLED_SLUG){
            str += " SLUG";
        }
        return str;
    };

    auto makeMagString = [&](){
        std::string str = "";
        if (this->activeWeapon.feedSystem == RELOAD_TYPE::DIRECT_LOAD){
            int roundCount = getInventoryAmmunitionCount(
                inventory, this->activeWeapon.cartridgeType
            );
            if (checkIsShotgun(this->activeWeapon.firearmType)){
                roundCount = getShotgunAmmunitionCount(this->inventory);
            }
            str = std::string(roundCount, '|');
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
        if (checkIsShotgun(this->activeWeapon.firearmType)){
            displayMags = getShotgunAmmunitionCount(this->inventory) > 0;
        }
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

    // Add explosive count to hud.
    auto invExplosive = getInventoryExplosiveSize(this->inventory);
    for (auto it = invExplosive.begin(); it != invExplosive.end(); ++it)
    {
        std::string label;
        switch (it->first){
            case EXPLOSIVE_TYPE::M67_GRENADE:
                label = "M67 grenades: ";
                break;
            case EXPLOSIVE_TYPE::M18A1_CLAYMORE:
                label = "claymores: ";
                break;
            case EXPLOSIVE_TYPE::M16_MINE:
                label = "M16 mines: ";
                break;
        }

        // Add newline to hud text if explosive info has not been added yet.
        if (it == invExplosive.begin()){
            this->hudText += "\n";
        }

        this->hudText += label + std::to_string(it->second);

        if (std::next(it) != invExplosive.end()){
            this->hudText += ", ";
        }
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

        // Always keep double-barreled shotgun chambered.
        if (activeWeapon.firearmType == FIREARM_TYPE::DB_SHOTGUN){
            activeWeapon.isChambered = true;
        }
    }
    else if (activeWeapon.isChambered){
        activeWeapon.chamberRoundType = activeWeapon.magazine.ammoType;
    }

    usleep(activeWeapon.shootIntervalMs * 1e3);
    canSwitchFirearm = true;
    activeWeapon.canShoot = activeWeapon.loadedRounds >= 1;

    // Switch shotgun to alternative ammo type if the current type is depleted
    // and the alternative ammo type is available in inventory.
    if (checkIsShotgun(this->activeWeapon.firearmType) &&
        this->activeWeapon.loadedRounds == 0)
    {
        CARTRIDGE_TYPE& cartridge = this->activeWeapon.cartridgeType;
        AMMO_TYPE& currAmmoType = this->activeWeapon.chamberRoundType;

        int currAmmoCount = getInventoryAmmunitionCount(
            this->inventory, cartridge
        );
        bool hasAltAmmo = checkHasAltShotgunAmmo(
            this->inventory, currAmmoType
        );

        if (currAmmoCount == 0 && hasAltAmmo){
            bool isBuckshot = cartridge ==
                             CARTRIDGE_TYPE::CARTRIDGE_12GA_BUCKSHOT;

            // Swap buckshot and slug.
            cartridge = isBuckshot ? CARTRIDGE_TYPE::CARTRIDGE_12GA_SLUG :
                        CARTRIDGE_TYPE::CARTRIDGE_12GA_BUCKSHOT;
            currAmmoType = isBuckshot ? AMMO_TYPE::RIFLED_SLUG :
                           AMMO_TYPE::PELLET_SPREAD;

            // Apply swapped ammo to magazine.
            this->activeWeapon.magazine = Magazine(
                cartridge, 2, this->activeWeapon.loadedRounds
            );
        }
    }
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
            if (!activeWeapon.isChambered){
                activeWeapon.magazine.cartridgeCount -= 1;
                activeWeapon.chamberRoundType = activeWeapon.magazine.ammoType;
                activeWeapon.isChambered = true;
            }
        }

        usleep(activeWeapon.chamberReloadDelay / 2.0 * 1e6);

        activeWeapon.isChambered = true;
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
        if (activeWeapon.loadedRounds >=
            activeWeapon.magazine.capacity + 1)
        {
            isReloading = false;
            activeWeapon.canShoot = true;
            return;
        }
        else if (activeWeapon.firearmType == FIREARM_TYPE::DB_SHOTGUN &&
                 activeWeapon.loadedRounds == 2)
        {
            this->isReloading = false;
            this->activeWeapon.canShoot = true;
            return;
        }

        usleep(activeWeapon.chamberReloadDelay / 2.0 * 1e6);

        usleep(activeWeapon.loadRoundTime.value() * 1e6);
        inventory.ammunition.at(cartridgeType) -= 1;
        activeWeapon.magazine.cartridgeCount += 1;
        activeWeapon.loadedRounds += 1;

        if (!activeWeapon.isChambered){
            activeWeapon.magazine.cartridgeCount -= 1;
            activeWeapon.isChambered = true;
        }

        usleep(activeWeapon.chamberReloadDelay / 2.0 * 1e6);

        activeWeapon.isChambered = true;
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
        activeWeapon.chamberRoundType = currentMag.ammoType;
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

void Player::plantM16Mine(World& world)
{
    bool hasMine = checkInventoryHasExplosiveType(
        this->inventory, EXPLOSIVE_TYPE::M16_MINE
    );

    if (!hasMine){
        return;
    }

    for (auto ex = inventory.explosives.begin();
         ex != inventory.explosives.end(); ++ex)
    {
        if (ex->explosiveType == EXPLOSIVE_TYPE::M16_MINE){
            ex->position = this->position;
            ex->_explosiveId = randInt();
            ex->isTriggerable = true;
            world.activeExplosives.push_back(*ex);
            this->inventory.explosives.erase(ex); // remove from inventory
            break;
        }
    }
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
                handleInventoryPickupExistingFirearm(
                    inventory, activeWeapon, item
                );
                continue;
            }

            CARTRIDGE_TYPE cartridge = activeWeapon.cartridgeType;
            bool hasExtraAmmo = checkHasMag(inventory, cartridge) ||
                                checkHasAmmunition(inventory, cartridge);

            // Equip firearm from supply drop if current firearm has no ammo.
            if (activeWeapon.loadedRounds == 0 && !hasExtraAmmo){
                activeWeapon = item;
                continue;
            }

            inventory.firearms.push_back(item);
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
    removeEmptyFirearmsFromInventory(inventory);
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


// Handles what should happen if the player picks up a firearm they already have.
void handleInventoryPickupExistingFirearm(
     Inventory& inventory, Firearm& activeWeapon, Firearm firearm)
{
    FIREARM_TYPE firearmType = firearm.firearmType;
    CARTRIDGE_TYPE cartridge = activeWeapon.cartridgeType;

    bool isDirectLoad = firearm.feedSystem == RELOAD_TYPE::DIRECT_LOAD;
    bool isActiveWeaponSameType = activeWeapon.firearmType == firearmType;
    bool inventoryHasFirearm = checkInventoryHasFirearm(inventory, firearmType);
    bool isEmpty = activeWeapon.loadedRounds == 0;
    bool hasAmmo = checkHasAmmunition(inventory, cartridge)
                   || checkHasMag(inventory, cartridge);

    // Take ammunition from firearm picked up.
    auto addAmmunition = [&](){
        if (isDirectLoad){
            CARTRIDGE_TYPE cartToAdd = firearm.cartridgeType;
            int newAmmoCount = getInventoryAmmunitionCount(
                inventory, cartToAdd
            );
            newAmmoCount += firearm.loadedRounds;
            inventory.ammunition[cartToAdd] = newAmmoCount;
        }
        else{
            inventory.magazines.push_back(firearm.magazine);
        }
    };

    if (isActiveWeaponSameType)
    {
        if (isEmpty){
            activeWeapon = firearm;
        }
        else{
            addAmmunition();
        }
    }
    else{
        // Remove existing firearm from inventory and add new one if it has
        // no loaded rounds.
        bool firearmWasRemoved = false;
        for (auto it = inventory.firearms.begin();
             it != inventory.firearms.end(); ++it)
        {
            if (it->firearmType == firearm.firearmType && it->loadedRounds == 0){
                inventory.firearms.erase(it);
                firearmWasRemoved = true;
                break;
            }
        }
        if (firearmWasRemoved){
            inventory.firearms.push_back(firearm);
        }
        else{
            addAmmunition();
        }
    }
}

bool checkIsShotgun(FIREARM_TYPE type)
{
    if (type == FIREARM_TYPE::BENELLI_M4 || type == FIREARM_TYPE::DB_SHOTGUN)
    {
        return true;
    }
    return false;
}
