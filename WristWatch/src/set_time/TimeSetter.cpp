#include <ctime>

#include <enum_helpers/EnumIteration.h>
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

void TimeSetter::incrementCursor()
{
    incrementEnum(timeCursor, TimeHelpers::SENTINEL);
}

void TimeSetter::decrementCursor()
{
    decrementEnum(timeCursor, TimeHelpers::SENTINEL);
}