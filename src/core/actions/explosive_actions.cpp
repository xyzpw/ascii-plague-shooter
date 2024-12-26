#include <thread>
#include <optional>
#include <utility>
#include "common.h"
#include "physics/grenade_physics.h"
#include "logic/infected_handler.h"

std::pair<int, int> getThrowVelocityRange(bool close);
std::pair<int, int> getThrowAngleRange(bool close);

void throwGrenade(World& world, Player& player, bool isCloseThrow)
{
    auto grenadeOptional = player.throwGrenade(world);
    if (!grenadeOptional.has_value()){
        return;
    }

    Explosive grenade = grenadeOptional.value();
    world.activeExplosives.push_back(grenade);

    auto [VEL_MIN, VEL_MAX] = getThrowVelocityRange(isCloseThrow);
    auto [ANGLE_MIN, ANGLE_MAX] = getThrowAngleRange(isCloseThrow);
    int velocity = randIntInRange(
        VEL_MIN, VEL_MAX
    );
    int angle = randIntInRange(
        ANGLE_MIN, ANGLE_MAX
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

std::pair<int, int> getThrowVelocityRange(bool close)
{
    int min = close ? PLAYER_THROW_VELOCITY_CLOSE_MIN :
                      PLAYER_THROW_VELOCITY_MIN;
    int max = close ? PLAYER_THROW_VELOCITY_CLOSE_MAX :
                      PLAYER_THROW_VELOCITY_MAX;

    std::pair<int, int> range = std::make_pair(min, max);
    return range;
}

std::pair<int, int> getThrowAngleRange(bool close)
{
    int min = close ? PLAYER_THROW_ANGLE_DEGREES_CLOSE_MIN :
                      PLAYER_THROW_ANGLE_DEGREES_MIN;
    int max = close ? PLAYER_THROW_ANGLE_DEGREES_CLOSE_MAX :
                      PLAYER_THROW_ANGLE_DEGREES_MAX;

    std::pair<int, int> range = std::make_pair(min, max);
    return range;
}
