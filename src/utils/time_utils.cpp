#include <chrono>
#include "time_utils.h"

double getEpochAsDecimal()
{
    auto temp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );
    double seconds = static_cast<double>(temp.count()) / 1e3;
    return seconds;
}

bool checkHasTimeElapsed(double epoch, double secondsCheck)
{
    double elapsed = getEpochAsDecimal() - epoch;
    return elapsed >= secondsCheck;
}
