#include <thread>
#include "controls/input/keyboard_input.h"
#include "controls/firearm_actions.h"
#include "controls/explosive_actions.h"
#include "controls/player_movement.h"
#include "world.h"
#include "entities/player.h"
#include "constants/input_constants.h"
#include "probability_utils.h"
#include "random_utils.h"
#include "logic/infected_spawning.h"

void respondToKeyPress (World& world, Player& player, int key)
{
    switch (key){
        case QUIT_KEY:
            world.active = false;
            break;
        case FORWARD_KEY:
            movePlayer(world, player, NORTH);
            break;
        case BACKWARD_KEY:
            movePlayer(world, player, SOUTH);
            break;
        case LEFT_KEY:
            movePlayer(world, player, WEST);
            break;
        case RIGHT_KEY:
            movePlayer(world, player, EAST);
            break;
        case SHOOT_KEY:{
            if (!player.activeWeapon.canShoot){
                return;
            }

            shootFirearm(world, player);

            // Small chance of spawning infected for each gunshot.
            if (player.activeWeapon.magazine.cartridgeType !=
                CARTRIDGE_TYPE::CARTRIDGE_22LR && checkProbability(0.04))
            {
                spawnInfected(world, 1);
            }
            break;
        }
        case RELOAD_KEY:{
            reloadFirearm(player);
            break;
        }
        case FAST_RELOAD_KEY:{
            fastReloadFirearm(player);
            break;
        }
        case GRENADE_KEY:{
            throwGrenade(world, player);
            break;
        }
        case CLAYMORE_KEY:{
            plantClaymore(world, player);
            break;
        }
        case PICKUP_KEY:{
            player.pickupItem(world);
            break;
        }
        case SWITCH_WEAPON_KEY:{
            player.switchFirearm();
            break;
        }
    }
};