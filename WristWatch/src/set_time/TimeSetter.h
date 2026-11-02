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
    /**
     * @brief Initializes the TimeSetter with an initial time setting.
     */
    void initialize(time_t initialTimeSetting);

    /**
     * @brief Returns a pointer to the local time structure based on the current time setting.
     */
    tm *getLocalTime();

    /**
     * @brief Returns positon of the time setter cursor, which indicates which part of the time is currently being set (hour, minute, second, etc.).
     */
    TimeHelpers::TimeCursor getTimeCursor();

    /**
     * @brief Increments the time cursor to the next position (e.g., from hour to minute). Wraps around to the beginning if it reaches the end.
     */
    void incrementCursor();

    /**
     * @brief Decrements the time cursor to the previous position (e.g., from minute to hour). Wraps around to the end if decrementing beyond the beginning.
     */
    void decrementCursor();
};

#endif