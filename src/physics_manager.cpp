#include <unistd.h>
#include "common.h"
#include "math_utils.h"
#include "game_utilities.h"

// Updates the grenades coordinates in real-time due to being thrown.
void processGrenadeThrow(World& world, Explosive grenade)
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
    int throwVelocity = randIntInRange(15, 20);
    int throwAngleDegrees = randIntInRange(40, 50);
    int throwDistance = computeThrownObjectRange(
        throwVelocity, throwAngleDegrees);

    auto maxCoordinate = getThrowCoordinates(world, grenade, throwDistance);
    if (!maxCoordinate.has_value()){
        return;
    }
    auto pathCoordinates = getThrowPathCoordinates(grenade.coordinates,
            grenade.facingDirection.value(), maxCoordinate.value());

    double airtime = 0.0;

    for (auto coord : pathCoordinates)
    {
        int index = getGrenadeIndex();
        world.activeExplosives.at(index).coordinates = coord;
        double velocity = getThrownObjectVelocityAtTime(
            throwVelocity, throwAngleDegrees, airtime
        );
        double sleepTime = 1.0 / velocity;
        usleep(sleepTime * 1e6);
        airtime += sleepTime;
    }
    world.activeExplosives.at(getGrenadeIndex()).coordinates = maxCoordinate.value();
}
