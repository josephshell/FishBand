#include <time_helpers/TimeHelpers.h>

#include <ctime>
#include <stdint.h>

time_t myTimegm(struct tm *tm);

void TimeHelpers::timestampAsTm(tm *localTime, time_t timestamp)
{
    *localTime = *std::gmtime(&timestamp);
}

time_t TimeHelpers::tmAsTimestamp(tm *timeTm)
{
#if defined(_WIN32) || defined(_WIN64)
    return _mkgmtime(timeTm);
#else
    return myTimegm(timeTm);
#endif
}

#include <time.h>
#include <stdint.h>

/* Converts broken-down UTC time to Unix timestamp (seconds since 1970-01-01) */
time_t myTimegm(struct tm *tm)
{
    int64_t year = (int64_t)tm->tm_year + 1900;
    int month = tm->tm_mon; // 0-11
    int day = tm->tm_mday;

    /* Normalize month overflow/underflow into years */
    if (month < 0 || month > 11)
    {
        year += month / 12;
        month %= 12;
        if (month < 0)
        {
            month += 12;
            year--;
        }
    }

    /* Shift March to month 0 to simplify leap-year math */
    month += 1; // 1-12
    year -= (month <= 2);
    int64_t era = (year >= 0 ? year : year - 399) / 400;
    unsigned yoe = (unsigned)(year - era * 400);                             // [0, 399]
    unsigned doy = (153 * (month + (month > 2 ? -3 : 9)) + 2) / 5 + day - 1; // [0, 365]
    unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;                    // [0, 146096]
    int64_t days = era * 146097 + (int64_t)doe - 719468;                     // Days since 1970-01-01

    int64_t total_seconds = days * 86400 + (int64_t)tm->tm_hour * 3600 + (int64_t)tm->tm_min * 60 + (int64_t)tm->tm_sec;

    time_t result = (time_t)total_seconds;

    /* Normalize struct tm fields (tm_wday, tm_yday, overflows) to match standard timegm */
#if defined(_WIN32)
    gmtime_s(tm, &result);
#else
    gmtime_r(&result, tm);
#endif

    return result;
}
