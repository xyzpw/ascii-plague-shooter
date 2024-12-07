#include <thread>
#include "controls/firearm_actions.h"
#include "world.h"
#include "entities/player.h"
#include "logic/infected_handler.h"

void shootFirearm(World& world, Player& player)
{
    std::thread(&Player::shootFirearm, &player).detach();
    handleFirearmShot(world, player);
}

void reloadFirearm(Player& player)
{
    std::thread(&Player::reloadFirearm, &player).detach();
}

void fastReloadFirearm(Player& player)
{
    std::thread(&Player::fastReloadFirearm, &player).detach();
}
