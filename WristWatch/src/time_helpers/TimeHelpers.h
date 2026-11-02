#ifndef TIME_HELPERS_H
#define TIME_HELPERS_H

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
}
#endif