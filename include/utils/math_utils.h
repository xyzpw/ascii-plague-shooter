#pragma once

#include "position.h"

double computeInverseSquareLaw(double value, double distance);
double getPositionDistance(Position, Position);
double computeAreaFromDistance(double distance);
double computeSectorAreaFromDistance(double distance, int degrees);
double getSectorWidthAtDistance(double distance, double degrees);
double getThrownObjectVelocityAtTime(int velocity, int degrees, double seconds);
double computeThrownObjectRange(int velocity, int degrees);
double calculateExpDecay(double value, double decay, double events);
