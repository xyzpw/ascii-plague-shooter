#pragma once

#include "injury_enums.h"

double calculateImpactFatalProbability(HIT_LOCATION location, int joules);
double calculateEarRuptureProbability(int pascals);
double calculateExplosionFatalProbability(int pascals);
double calculateFragmentFatalProbability(int joules, int fragments);