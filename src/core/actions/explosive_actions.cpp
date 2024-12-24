#include <thread>
#include <optional>
#include "common.h"
#include "physics/grenade_physics.h"
#include "logic/infected_handler.h"

void throwGrenade(World& world, Player& player)
{
    auto grenadeOptional = player.throwGrenade(world);
    if (!grenadeOptional.has_value()){
        return;
    }

    Explosive grenade = grenadeOptional.value();
    world.activeExplosives.push_back(grenade);

    int velocity = randIntInRange(
        PLAYER_THROW_VELOCITY_MIN, PLAYER_THROW_VELOCITY_MAX
    );
    int angle = randIntInRange(
        PLAYER_THROW_ANGLE_DEGREES_MIN, PLAYER_THROW_ANGLE_DEGREES_MAX
    );

    std::thread(
        processGrenadeThrow, std::ref(world), grenade, velocity, angle
    ).detach();
    std::thread(
        handleGrenadeExplosion,
        std::ref(world),
        std::ref(player),
        grenade._explosiveId.value()
    ).detach();
}

void plantClaymore(World& world, Player& player)
{
    if (player.hasPlantedClaymore){
        Explosive claymore(EXPLOSIVE_TYPE::__COUNT);
        for (auto cl : world.activeExplosives){
            if (cl.explosiveType == EXPLOSIVE_TYPE::M18A1_CLAYMORE){
                claymore = cl;
                break;
            }
        }
        std::thread(
            handleClaymoreExplosion,
            std::ref(world),
            std::ref(player),
            claymore
        ).detach();
    }
    else{
        player.plantClaymore(world);
    }
}
