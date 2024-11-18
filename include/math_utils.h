#pragma once

#include <iostream>
#include <utility>

constexpr double PI = 3.141592653589793;
constexpr double GRAVITY_ACCELERATION = 9.80665;

double computeEardrumRuptureProb(int pascals);
double computeExplosionLethalProb(int pascals);
double computeInverseSquareLaw(double value, double distance);
double getMapPointsDistance(std::pair<int, int> coord1, std::pair<int, int> coord2);
double computeFragmentLethalProb(int joules, int fragments);
double computeAreaFromDistance(double distance);
double computeSectorAreaFromDistance(double distance, int degrees);
double getSectorWidthAtDistance(double distance, int degrees);
double getThrownObjectVelocityAtTime(int velocity, int degrees, double seconds);
double computeThrownObjectRange(int velocity, int degrees);
