#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include "inventory_utils.h"
#include "entities/inventory.h"
#include "weapon_enums.h"

int getInventoryMagCount(Inventory inventory, CARTRIDGE_TYPE type)
{
    int count = 0;

    for (auto mag : inventory.magazines){
        if (mag.cartridgeType == type){
            ++count;
        }
    }

    return count;
}

int getInventoryAmmunitionCount(Inventory& inventory, CARTRIDGE_TYPE cartridge)
{
    int count = 0;

    if (inventory.ammunition.count(cartridge)){
        count = inventory.ammunition.at(cartridge);
    }
    return count;
}

bool checkInventoryHasFirearm(Inventory inventory, FIREARM_TYPE type)
{
    for (auto firearm : inventory.firearms){
        if (firearm.firearmType == type){
            return true;
        }
    }
    return false;
}

void removeEmptyFirearmsFromInventory(Inventory& inventory)
{
    std::vector<Firearm>& firearms = inventory.firearms;

    auto it = std::remove_if(firearms.begin(), firearms.end(), [&](Firearm f){
        CARTRIDGE_TYPE cartridge = f.cartridgeType;

        bool isEmpty = f.loadedRounds == 0;
        bool hasAmmo = getInventoryAmmunitionCount(inventory, cartridge);
        bool hasMags = getInventoryMagCount(inventory, cartridge);

        return isEmpty && !hasAmmo && !hasMags;
    });

    firearms.erase(it, firearms.end());
}

/*
 Returns an unordered map of explosive type and how many of which are in the
 specified inventory.
*/
std::unordered_map<EXPLOSIVE_TYPE, int> getInventoryExplosiveSize(
                                        Inventory inventory)
{
    std::unordered_map<EXPLOSIVE_TYPE, int> explosives{};
    std::unordered_set<EXPLOSIVE_TYPE> invExplosiveSet;

    for (auto ex : inventory.explosives)
    {
        if (invExplosiveSet.find(ex.explosiveType) == invExplosiveSet.end())
        {
            explosives[ex.explosiveType] = 1;
            invExplosiveSet.insert(ex.explosiveType);
        }
        else
        {
            explosives[ex.explosiveType] += 1;
        }
    }
    return explosives;
}

bool checkInventoryHasExplosiveType(Inventory& inventory,
                                    const EXPLOSIVE_TYPE type)
{
    for (auto& it : inventory.explosives)
    {
        if (it.explosiveType == type){
            return true;
        }
    }
    return false;
}

bool checkHasAltShotgunAmmo(Inventory& inventory, AMMO_TYPE currentAmmoType)
{
    AMMO_TYPE buckshot = AMMO_TYPE::PELLET_SPREAD;
    AMMO_TYPE slug = AMMO_TYPE::RIFLED_SLUG;

    int count = 0;
    if (currentAmmoType == buckshot){
        count = getInventoryAmmunitionCount(
            inventory, CARTRIDGE_TYPE::CARTRIDGE_12GA_SLUG
        );
    }
    else if (currentAmmoType == slug){
        count = getInventoryAmmunitionCount(
            inventory, CARTRIDGE_TYPE::CARTRIDGE_12GA_BUCKSHOT
        );
    }

    return count > 0;
}

int getShotgunAmmunitionCount(Inventory& inventory)
{
    int count = 0;
    count += getInventoryAmmunitionCount(
        inventory, CARTRIDGE_TYPE::CARTRIDGE_12GA_BUCKSHOT
    );
    count += getInventoryAmmunitionCount(
        inventory, CARTRIDGE_TYPE::CARTRIDGE_12GA_SLUG
    );

    return count;
}
