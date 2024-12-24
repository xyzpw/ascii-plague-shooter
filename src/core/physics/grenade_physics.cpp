#include <unistd.h>
#include "physics/grenade_physics.h"
#include "physics/throw_physics.h"
#include "world.h"
#include "entities/explosive.h"
#include "math_utils.h"
#include "random_utils.h"
#include "constants/player_constants.h"

// Updates the grenades coordinates in real-time due to being thrown.
void processGrenadeThrow(World& world, Explosive grenade,
                         int throwVelocity, int throwAngleDegrees)
{
    auto getGrenadeIndex = [&](){
        int index = world.activeExplosives.size();
        for (int i = 0; i < world.activeExplosives.size(); ++i){
            if (world.activeExplosives.at(i)._explosiveId.has_value() &&
                    world.activeExplosives.at(i)._explosiveId.value() ==
                    grenade._explosiveId.value())
            {
                index = i;
            }
        }
        return index;
    };
    int throwDistance = computeThrownObjectRange(
        throwVelocity, throwAngleDegrees);

    auto maxPosition = getThrowPosition(world, grenade, throwDistance);
    if (!maxPosition.has_value()){
        return;
    }
    auto pathPositions = getThrowPathPositions(
        grenade.position,
        grenade.facingDirection.value(),
        maxPosition.value()
    );

    double airtime = 0.0;

    for (auto pos : pathPositions)
    {
        int index = getGrenadeIndex();
        world.activeExplosives.at(index).position = pos;
        double velocity = getThrownObjectVelocityAtTime(
            throwVelocity, throwAngleDegrees, airtime
        );
        double sleepTime = 1.0 / velocity;
        usleep(sleepTime * 1e6);
        airtime += sleepTime;
    }
    world.activeExplosives.at(getGrenadeIndex()).position = maxPosition.value();
}