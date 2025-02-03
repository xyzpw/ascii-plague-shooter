#include <unistd.h>
#include <thread>
#include "animations/explosion_animation.h"
#include "constants/explosive_constants.h"
#include "world.h"
#include "position.h"
#include "utils/math_utils.h"
#include "utils/game_utils.h"

void playExplosionAnimation(World& world, Position pos, double energy)
{
    double animationDuration = 0.1;
    double timeRemaining = animationDuration;

    while (timeRemaining > 0.0)
    {
        double elapsed = animationDuration - timeRemaining;
        int radius = getBlastWaveRadius(energy, elapsed);

        auto points = getMidpointCirclePositions(
            pos, radius
        );

        for (auto p : points)
        {
            if (checkPositionInsideMap(world, p)){
                world.animationPositions.push_back(p);
            }
        }

        usleep(0.016 * 1e+6);
        timeRemaining -= 0.016;

        world.animationPositions = {};
    }
}

void playExplosionAnimationThread(World& world, Position pos, double energy)
{
    std::thread t(playExplosionAnimation, std::ref(world), pos, energy);
    t.detach();
}
