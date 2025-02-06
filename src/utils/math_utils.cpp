#include <cmath>
#include <vector>
#include <algorithm>
#include "math_utils.h"
#include "position.h"
#include "world_enums.h"
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

/*
    Get midpoint circle positions and filter them to only be
    the ones within an arc of specified degrees and direction.
*/
std::vector<Position> getMidpointCircleArcPositions(
        Position pos, DIRECTION direction, int radius, double degrees)
{
    std::vector<Position> arcPoints;
    std::vector<Position> circlePoints = getMidpointCirclePositions(
        pos, radius
    );

    int degBase;
    if (direction == NORTH || direction == SOUTH){
        degBase = direction == NORTH ? 90 : 270;
    }
    else {
        degBase = direction == EAST ? 0 : 180;
    }
    double degLow = degBase - degrees/2.0;
    double degHigh = degBase + degrees/2.0;

    if (degLow < 0)
        degLow += 360;
    if (degHigh >= 360)
        degHigh -= 360;

    auto isInArc = [&](const Position& p){
        double angle = std::atan2(
            pos.row - p.row,
            p.column - pos.column
        ) * (180.0 / PI);
        if (angle < 0) angle += 360.0;

        if (degLow < degHigh){
            return angle >= degLow && angle <= degHigh;
        }
        else {
            return angle >= degLow || angle <= degHigh;
        }
    };

    std::copy_if(
        circlePoints.begin(), circlePoints.end(),
        std::back_inserter(arcPoints), isInArc
    );

    return arcPoints;
}

// Calculate the pascals of an explosion with the energy and distance.
double calculatePascals(double energy, double distance)
{
    if (distance == 0) distance = 0.5;

    double netExplosiveMass = energy / 4184e+3;
    double scaledDistance = distance / std::cbrt(netExplosiveMass);

    double mPa;
    if (scaledDistance < 8){
        mPa = 1700;
        mPa *= std::exp(-7.14 * std::pow(scaledDistance, 0.28));
        mPa += 0.0156;
    }
    else {
        mPa = 8000;
        mPa *= std::exp(-10.46 * std::pow(scaledDistance, 0.1));
    }

    return mPa * 1e+6;
}
