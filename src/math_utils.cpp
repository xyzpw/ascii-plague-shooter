#include <cmath>
#include "math_utils.h"

double computeInverseSquareLaw(double value, double distance)
{
    double denominator = 4 * PI * std::pow(distance, 2);
    double newValue = denominator == 0 ? value : 1.0 / denominator * value;
    return newValue;
}

double computeAreaFromDistance(double distance)
{
    double area = PI * std::pow(distance, 2);
    return area;
}

double computeSectorAreaFromDistance(double distance, int degrees)
{
    double radians = degrees * PI / 180;
    double area = radians / 2 * std::pow(distance, 2);
    return area;
}

double getSectorWidthAtDistance(double distance, int degrees)
{
    double radians = degrees * PI / 180;
    double width = std::tan(radians / 2) * 2 * distance;
    return width;
}

double getThrownObjectVelocityAtTime(int velocity, int degrees, double seconds)
{
    double radians = degrees * PI / 180.0;

    double v0x = velocity * std::cos(radians);
    double v0y = velocity * std::sin(radians);

    double currentVy = v0y - GRAVITY_ACCELERATION * seconds;
    double currentVx = v0x;

    double currentVelocity = std::sqrt(
        currentVx * currentVx + currentVy * currentVy
    );

    return currentVelocity;
}

double computeThrownObjectRange(int velocity, int degrees)
{
    double radians = degrees * PI / 180.0;
    double range = std::pow(velocity, 2) * std::sin(radians * 2) /
                   GRAVITY_ACCELERATION;
    return range;
}
