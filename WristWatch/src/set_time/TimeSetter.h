#ifndef TIME_SETTER_H
#define TIME_SETTER_H

#include <ctime>

#include <time_helpers/TimeHelpers.h>

class TimeSetter
{
private:
    time_t currentTimeSetting;
    TimeHelpers::TimeCursor timeCursor;

public:
    void initialize(time_t initialTimeSetting);

    tm *getLocalTime();

    TimeHelpers::TimeCursor getTimeCursor();
};

#endif