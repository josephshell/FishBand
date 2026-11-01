#include <ctime>

#include <set_time/TimeSetter.h>

tm localTime;

void TimeSetter::initialize(time_t initialTimeSetting)
{
    currentTimeSetting = initialTimeSetting;
}

tm* TimeSetter::getLocalTime()
{
    #if defined(_WIN32)
        localtime_s(&currentTimeSetting, &localTime);
#else
        localtime_r(&currentTimeSetting, &localTime);
    #endif
    return &localTime;
}

TimeHelpers::TimeCursor TimeSetter::getTimeCursor()
{
    return timeCursor;
}