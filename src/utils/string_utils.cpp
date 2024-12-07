#include <string>
#include "string_utils.h"

std::string makeClockString(int seconds)
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
