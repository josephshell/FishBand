#include <alarms/Alarms.h>

const char *getFrequencyStr(AlarmFrequency freq)
{
    switch (freq)
    {
    case EVDY:
        return "EVDY";
    case WKDY:
        return "WKDY";
    case WKND:
        return "WKND";
    case ONCE:
        return "ONCE";
    default:
        return "UH..";
    }
}