#include <unistd.h>
#include <thread>
#include "animations/explosion_animation.h"
#include "constants/explosive_constants.h"
#include "world.h"
#include "position.h"
#include "entities/explosive.h"
#include "world_enums.h"
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

void playClaymoreExplosionAnimation(World& world, Explosive claymore)
{
    double animationDuration = 0.13;
    double timeRemaining = animationDuration;

    std::vector<Position>& animPositions = world.animationPositions;

    int claymoreEnergy = claymore.explosionEnergy;
    int arcDeg = CLAYMORE_FRAGMENT_DEGREES;
    Position& claymorePos = claymore.position;
    if (!claymore.facingDirection.has_value()){
        return;
    }
    DIRECTION& claymoreDirection = claymore.facingDirection.value();

    while (timeRemaining > 0.0)
    {
        double elapsed = animationDuration - timeRemaining;
        int radius = getBlastWaveRadius(claymoreEnergy, elapsed);

        auto arcPoints = getMidpointCircleArcPositions(
            claymorePos, claymoreDirection, radius, arcDeg
        );
        for (auto p : arcPoints)
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

void playClaymoreExplosionAnimThread(World& world, Explosive claymore)
{
    std::thread t(
        playClaymoreExplosionAnimation, std::ref(world), claymore
    );
    t.detach();
}
