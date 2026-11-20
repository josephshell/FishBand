#ifndef TIME_HELPERS_H
#define TIME_HELPERS_H

#include <ctime>

namespace TimeHelpers
{
    enum TimeCursor
    {
        HOUR,
        MINUTE,
        SECOND,
        MONTH,
        DAY,
        YEAR,
        CANCEL,
        DONE,
        SENTINEL // Indicates end of enum, not a valid value
    };

    void timestampAsTm(tm *localTime, time_t currentTime);

    time_t tmAsTimestamp(tm *timeTm);
}
#endif