#include <time_helpers/TimeHelpers.h>

void TimeHelpers::getLocalTime(tm *localTime, time_t currentTime)
{
    #if defined(_WIN32)
        localtime_s(localTime, &currentTime);
    #else
        localtime_r(&currentTime, localTime);
    #endif
}