#ifndef TIME_SETTER_H
#define TIME_SETTER_H

#include <ctime>

#include <time_helpers/TimeHelpers.h>

class TimeSetter
{
private:
    time_t currentTimeSetting = 0;
    TimeHelpers::TimeCursor timeCursor = TimeHelpers::HOUR;

public:
    /**
     * @brief Returns the current time setting as a time_t value.
     */
    time_t getCurrentTimeSetting() const
    {
        return currentTimeSetting;
    }

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

    /**
     * @brief Increments the value of the time setting at the current cursor position (e.g., increases the hour, minute, etc.).
     *
     * @return 0 if the increment was successful, 1 if the user just requested to save the time, and -1 if the user just requested to cancel setting the time
     */
    int incrementTimeSetting();

    /**
     * @brief Decrements the value of the time setting at the current cursor position (e.g., decreases the hour, minute, etc.).
     */
    void decrementTimeSetting();
};

#endif