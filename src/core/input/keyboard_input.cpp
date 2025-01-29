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
        case '\b':
            world.active = false;
            break;
        case FORWARD_KEY:
        case KEY_UP:
            movePlayer(world, player, NORTH);
            break;
        case LOOK_FORWARD_KEY:
            player.facingDirection = NORTH;
            player.fixWeaponAppearance();
            break;
        case BACKWARD_KEY:
        case KEY_DOWN:
            movePlayer(world, player, SOUTH);
            break;
        case LOOK_BACKWARD_KEY:
            player.facingDirection = SOUTH;
            player.fixWeaponAppearance();
            break;
        case LEFT_KEY:
        case KEY_LEFT:
            movePlayer(world, player, WEST);
            break;
        case LOOK_LEFT_KEY:
            player.facingDirection = WEST;
            player.fixWeaponAppearance();
            break;
        case RIGHT_KEY:
        case KEY_RIGHT:
            movePlayer(world, player, EAST);
            break;
        case LOOK_RIGHT_KEY:
            player.facingDirection = EAST;
            player.fixWeaponAppearance();
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
        case GRENADE_KEY_CLOSE:{
            throwGrenade(world, player, true);
            break;
        }
        case CLAYMORE_KEY:{
            plantClaymore(world, player);
            break;
        }
        case M16_MINE_KEY:{
            player.plantM16Mine(world);
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
