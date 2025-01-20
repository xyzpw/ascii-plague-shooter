#include <cmath>
#include <vector>
#include "math_utils.h"
#include "position.h"
#include "constants/physics_constants.h"
#include "constants/math_constants.h"

double computeInverseSquareLaw(double value, double distance)
{
    double denominator = 4 * PI * std::pow(distance, 2);
    double newValue = denominator == 0 ? value : 1.0 / denominator * value;
    return newValue;
}

double computeAreaFromDistance(double distance)
{
    double area = 4 * PI * std::pow(distance, 2);
    return area;
}

double computeSectorAreaFromDistance(double distance, int degrees)
{
    double radians = degrees * PI / 180;
    double area = radians / 2 * std::pow(distance, 2);
    return area;
}

double getSectorWidthAtDistance(double distance, double degrees)
{
    double radians = degrees * PI / 180;
    double width = std::sin(radians / 2.0) * 2 * distance;
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

double getPositionDistance(Position p1, Position p2)
{
    int colDistance = std::abs(p2.column - p1.column);
    int rowDistance = std::abs(p2.row - p1.row);

    double distance = std::sqrt(
        std::pow(colDistance, 2) + std::pow(rowDistance, 2)
    );

    return distance;
}

double calculateExpDecay(double value, double decay, double events)
{
    return value * std::pow(1 - decay, events);
}

double getBlastWaveRadius(double energy, double time)
{
    return std::pow(energy * time * time / AIR_DENSITY, 1.0/5.0);
}

std::vector<Position> getMidpointCirclePositions(Position pos, int radius)
{
    std::vector<Position> points;

    int col = pos.column;
    int row = pos.row;

    int x = 0;
    int y = radius;
    int d = 1 - radius;

    auto addPoints = [&](int x, int y){
        points.push_back(Position{.column=col+x, .row=row+y});
        points.push_back(Position{.column=col-x, .row=row+y});
        points.push_back(Position{.column=col+x, .row=row-y});
        points.push_back(Position{.column=col-x, .row=row-y});
        points.push_back(Position{.column=col+y, .row=row+x});
        points.push_back(Position{.column=col-y, .row=row+x});
        points.push_back(Position{.column=col+y, .row=row-x});
        points.push_back(Position{.column=col-y, .row=row-x});
    };

    addPoints(x, y);
    while (x < y)
    {
        if (d < 0){
            d += 2 * x + 3;
        }
        else {
            d += 2 * (x - y) + 5;
            --y;
        }
        ++x;
        addPoints(x, y);
    }

    return points;
}
