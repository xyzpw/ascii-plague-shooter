#pragma once

#include <utility>
#include <string>
#include <vector>
#include <optional>
#include "common.h"

std::pair<std::pair<int, int>, std::pair<int, int>> getMapLimits(
    std::pair<int, int> termSize
);
void playAudio(std::string filename);
std::pair<int, int> getTerminalSize();
void initializeAudio();
void cleanupAudio();
int getEpochMs();
int computeCoordinatesChange(
    std::pair<int, int> coord1, std::pair<int, int> coord2, bool horizontal
);
std::optional<std::pair<int,int>> getThrowCoordinates(
    World, Explosive, int distance
);
std::vector<std::pair<int,int>> getThrowPathCoordinates(
    std::pair<int,int> startCoord, DIRECTION direction,
    std::pair<int,int> endCoord);

bool checkCoordinatesInsideMap(World world, std::pair<int, int> coordinates);
std::string makeClockString(time_t seconds);
std::pair<int, int> getRandMapCoordinates(World world);
int randInt();
int randIntInRange(int min, int max);
bool checkProbability(double);
