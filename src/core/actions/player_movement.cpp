#include "controls/player_movement.h"
#include "common.h"

void movePlayer(World& world, Player& player, DIRECTION direction)
{
    auto fixPlayerDirection = [&](DIRECTION direction){
        if (player.facingDirection != direction){
            player.facingDirection = direction;
        }
    };

    switch (direction){
        case DIRECTION::NORTH:
            if (player.position.row - 1 >= world.mapRowLimits.first){
                player.position.row -= 1;
                fixPlayerDirection(NORTH);
                player.fixWeaponAppearance();
            }
            break;
        case DIRECTION::SOUTH:
            if (player.position.row + 1 <= world.mapRowLimits.second){
                player.position.row += 1;
                fixPlayerDirection(SOUTH);
                player.fixWeaponAppearance();
            }
            break;
        case DIRECTION::EAST:
            if (player.position.column + 1 <= world.mapColumnLimits.second){
                player.position.column += 1;
                fixPlayerDirection(EAST);
                player.fixWeaponAppearance();
            }
            break;
        case DIRECTION::WEST:
            if (player.position.column - 1 >= world.mapColumnLimits.first){
                player.position.column -= 1;
                fixPlayerDirection(WEST);
                player.fixWeaponAppearance();
            }
            break;
    }
}
