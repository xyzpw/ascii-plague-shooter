#include <iostream>
#include <utility>
#include <algorithm>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <mutex>
#include <thread>
#include <random>
#include "common.h"
#include <sys/ioctl.h>
#include <condition_variable>
#include <chrono>
#include "math_utils.h"
#include "game_utilities.h"

std::mt19937 mtRandGen(std::random_device{}());

std::uniform_int_distribution<int> uniformIntDist(
    0, std::numeric_limits<int>::max()
);

std::mutex audioMutex;
std::condition_variable audioFinishCondition;
bool isAudioPlaying = false;

double getMapPointsDistance(
       std::pair<int, int> coord1, std::pair<int, int> coord2)
{
    int colDistance = std::abs(coord2.first - coord1.first);
    int rowDistance = std::abs(coord2.second - coord1.second);

    double distance = std::sqrt(
        std::pow(colDistance, 2) + std::pow(rowDistance, 2)
    );

    return distance;
}

std::vector<std::pair<int,int>> getThrowPathCoordinates(
        std::pair<int,int> startCoord, DIRECTION direction,
        std::pair<int,int> endCoord)
{
    std::vector<std::pair<int,int>> pathCoords{};
    int incrementValue = direction == EAST || direction == SOUTH ? +1 : -1;
    bool isVertical = direction == NORTH || direction == SOUTH;

    if (isVertical){
        for (int i = startCoord.second + incrementValue; i != endCoord.second;
             i += incrementValue)
        {
            std::pair<int, int> _c = std::make_pair(startCoord.first, i);
            pathCoords.push_back(_c);
        }
    }
    else {
        for (int i = startCoord.first + incrementValue; i != endCoord.first;
             i += incrementValue)
        {
            std::pair<int, int> _c = std::make_pair(i, startCoord.second);
            pathCoords.push_back(_c);
        }
    }
    return pathCoords;
}

// Coordinates of where a thrown explosive will land.
std::optional<std::pair<int,int>> getThrowCoordinates(
        World world, Explosive explosive, int distance)
{
    if (!explosive.facingDirection.has_value()){
        return std::nullopt;
    }

    auto direction = explosive.facingDirection.value();
    std::optional<std::pair<int,int>> landCoordinates;
    bool isVertical = direction == NORTH || direction == SOUTH;

    if (isVertical){
        int rowIncrease = direction == SOUTH ?  distance : 0 - distance;
        int newRow = std::clamp(explosive.coordinates.second + rowIncrease,
                world.mapRowLimits.first, world.mapRowLimits.second);
        landCoordinates = std::make_pair(explosive.coordinates.first, newRow);
    }
    else{
        int colIncrease = direction == EAST ? distance : 0 - distance;
        int newCol = std::clamp(colIncrease + explosive.coordinates.first,
                world.mapColumnLimits.first, world.mapColumnLimits.second);
        landCoordinates = std::make_pair(newCol, explosive.coordinates.second);
    }
    return landCoordinates;
}

int computeCoordinatesChange(
    std::pair<int, int> coord1, std::pair<int, int> coord2, bool horizontal)
{
    int diff;

    if (horizontal){
        diff = std::abs(coord1.first - coord2.first);
    }
    else {
        diff = std::abs(coord1.second - coord2.second);
    }

    return diff;
}

// Returns the limits where characters can be move on the terminal.
std::pair<std::pair<int, int>, std::pair<int, int>> getMapLimits(
    std::pair<int, int> termSize)
{
    //NOTE: Column 1 is treated as column 0 with ncurses.
    //NOTE: We want to be 2 spaces from the borders.

    auto [columns, rows] = termSize;
    int centerCol = columns / 2, centerRow = rows / 2;

    int colLeftLimit = std::max(centerCol - 80/2, 3);
    int colRightLimit = std::min(centerCol + 80/2, termSize.first - 2);
    int rowTopLimit = std::max(centerRow - 24/2, 7);
    int rowBottomLimit = std::min(centerRow + 24/2, termSize.second - 2);

    std::pair<std::pair<int, int>, std::pair<int, int>> limits =
        {
            std::make_pair(colLeftLimit - 1, colRightLimit - 1),
            std::make_pair(rowTopLimit - 1, rowBottomLimit - 1),
        };

    return limits;
}

bool checkCoordinatesInsideMap(World world, std::pair<int, int> coordinates)
{
    bool colTooLow = coordinates.first < world.mapColumnLimits.first;
    bool colTooHigh = coordinates.first > world.mapColumnLimits.second;
    bool rowTooLow = coordinates.second < world.mapRowLimits.first;
    bool rowTooHigh = coordinates.second > world.mapRowLimits.second;

    if (colTooLow || colTooHigh || rowTooLow || rowTooHigh)
        return false;

    return true;
}

std::pair<int, int> getTerminalSize()
{
    struct winsize wSize;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &wSize);
    std::pair<int, int> size = std::make_pair(wSize.ws_col, wSize.ws_row);
    return size;
}

std::string makeClockString(time_t seconds)
{
    auto fillClock = [](int num){
        if (num < 10){
            return "0" + std::to_string(num);
        }

        return std::to_string(num);
    };

    int clockMinutes = seconds / 60;
    int clockSeconds = seconds - clockMinutes * 60;

    return fillClock(clockMinutes) + ":" + fillClock(clockSeconds);
}

std::pair<int, int> getRandMapCoordinates(World world)
{
    std::pair<int, int> colLimits = world.mapColumnLimits;
    std::pair<int, int> rowLimits = world.mapRowLimits;

    int col = randIntInRange(colLimits.first, colLimits.second);
    int row = randIntInRange(rowLimits.first, rowLimits.second);
    return std::make_pair(col, row);
}

int getEpochMs()
{
    auto msObj = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    time_t ms = msObj.count();
    return ms;
}

int randInt()
{
    return uniformIntDist(mtRandGen);
}

int randIntInRange(int min, int max)
{
    if (min > max){
        std::swap(min, max);
    }

    int range = max - min + 1;

    int nMax = std::numeric_limits<int>::max() / range;

    int result;
    do {
        result = randInt();
    } while (result >= nMax * range);
    return result % range + min;
}

/*
 Generates a random number from 0 to 1 and returns true if it is less than
 or equal to the specified probability.
*/
bool checkProbability(double p)
{
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double rProb = dist(mtRandGen);
    return rProb <= p;
}

void playAudio(std::string filename)
{
    if (filename == ""){
        return;
    }

    {
        std::lock_guard<std::mutex> lock(audioMutex);
        isAudioPlaying = true;
    }
    std::string location = std::string("assets/") + filename;
    SDL_AudioSpec wavSpec;
    Uint8* wavBuf;
    Uint32 wavLen;

    if (SDL_LoadWAV(location.c_str(), &wavSpec, &wavBuf, &wavLen) == NULL)
    {
        {
            std::lock_guard<std::mutex> lock(audioMutex);
            isAudioPlaying = false;
        }
        audioFinishCondition.notify_all();
        return;
    }

    SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
    SDL_QueueAudio(device, wavBuf, wavLen);
    SDL_PauseAudioDevice(device, 0);

    while (SDL_GetQueuedAudioSize(device) > 0)
        SDL_Delay(10);

    {
        std::lock_guard<std::mutex> lock(audioMutex);
        try{
            SDL_CloseAudioDevice(device);
            SDL_FreeWAV(wavBuf);
        } catch(...){
            audioFinishCondition.notify_all();
        }
        isAudioPlaying = false;
    }
    audioFinishCondition.notify_all();
}

void initializeAudio()
{
    SDL_Init(SDL_INIT_AUDIO);
}
void cleanupAudio()
{
    std::unique_lock<std::mutex> lock(audioMutex);
    audioFinishCondition.wait(lock, [] { return !isAudioPlaying; });
    SDL_Quit();
}
