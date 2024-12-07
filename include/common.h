#pragma once

/*
    This file is used to include common headers without having to do so on
    each file.
    Headers include:
        world
        enums
        position and splatter
        entities, constants, and utils.
*/

#include "world.h"
#include "position.h"
#include "splatter_effect.h"
#include "injury_enums.h"
#include "weapon_enums.h"
#include "world_enums.h"

#include "entities/explosive.h"
#include "entities/firearm.h"
#include "entities/game_stats.h"
#include "entities/infected.h"
#include "entities/inventory.h"
#include "entities/magazine.h"
#include "entities/player.h"
#include "entities/rescue.h"
#include "entities/supply_drop.h"

#include "constants/explosive_constants.h"
#include "constants/firearm_constants.h"
#include "constants/infected_constants.h"
#include "constants/injury_constants.h"
#include "constants/input_constants.h"
#include "constants/math_constants.h"
#include "constants/physics_constants.h"
#include "constants/player_constants.h"
#include "constants/world_constants.h"

#include "game_utils.h"
#include "injury_utils.h"
#include "inventory_utils.h"
#include "math_utils.h"
#include "player_utils.h"
#include "probability_utils.h"
#include "random_utils.h"
#include "string_utils.h"
#include "time_utils.h"


void playGame(World world);
