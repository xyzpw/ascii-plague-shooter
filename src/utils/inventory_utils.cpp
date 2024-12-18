#include <unordered_map>
#include <unordered_set>
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
