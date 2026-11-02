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
        localtime_s(&localTime, &currentTimeSetting);
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

void TimeSetter::incrementTimeSetting()
{
    tm* localTimePtr = getLocalTime();
    switch (timeCursor)
    {
        case TimeHelpers::HOUR:
            localTimePtr->tm_hour = (localTimePtr->tm_hour + 1) % 24;
            break;
        case TimeHelpers::MINUTE:
            localTimePtr->tm_min = (localTimePtr->tm_min + 1) % 60;
            break;
        case TimeHelpers::SECOND:
            localTimePtr->tm_sec = (localTimePtr->tm_sec + 1) % 60;
            break;
        case TimeHelpers::MONTH:
            localTimePtr->tm_mon = (localTimePtr->tm_mon + 1) % 12;
            break;
        case TimeHelpers::DAY:
            localTimePtr->tm_mday++;
            break;
        case TimeHelpers::YEAR:
            localTimePtr->tm_year++;
            break;
        case TimeHelpers::CANCEL:
            // Handle cancel action 
            break;
        case TimeHelpers::DONE:
            // Handle done action 
            break;
        default:
            break;
    }
    currentTimeSetting = mktime(localTimePtr);
}

void TimeSetter::decrementTimeSetting()
{
    tm* localTimePtr = getLocalTime();
    switch (timeCursor)
    {
        case TimeHelpers::HOUR:
            localTimePtr->tm_hour = (localTimePtr->tm_hour + 23) % 24;
            break;
        case TimeHelpers::MINUTE:
            localTimePtr->tm_min = (localTimePtr->tm_min + 59) % 60;
            break;
        case TimeHelpers::SECOND:
            localTimePtr->tm_sec = (localTimePtr->tm_sec + 59) % 60;
            break;
        case TimeHelpers::MONTH:
            localTimePtr->tm_mon = (localTimePtr->tm_mon + 11) % 12;
            break;
        case TimeHelpers::DAY:
            localTimePtr->tm_mday--;
            break;
        case TimeHelpers::YEAR:
            localTimePtr->tm_year--;
            break;
        case TimeHelpers::CANCEL:
        case TimeHelpers::DONE:
            // Handle cancel and done actions, if necessary
            break;
        default:
            break;
    }
    currentTimeSetting = mktime(localTimePtr);
}