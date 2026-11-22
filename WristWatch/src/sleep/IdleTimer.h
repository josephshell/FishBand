#ifndef IDLE_TIMER_H
#define IDLE_TIMER_H

#include <ctime>
#include <time_provider/TimeProvider.h>

class IdleTimer
{
private:
    // Seconds of idle until user considered inactive
    long idleTimeout = 30;
    // Last noted activity timestamp
    time_t lastActivityTime = 0;
    // Time provider for notiing times and checking for idle status
    TimeProvider *tp;

public:
    IdleTimer(long idleTimeout, TimeProvider *tp) : idleTimeout(idleTimeout), tp(tp) {};
    /**
     * @brief Checks if the user has been idle for [idleTimeout] seconds
     *
     * @returns true if user has reached idle timeout, false if not
     */
    bool isUserIdle();
    /**
     * @brief Notes activity by the user, restarting the idle timer
     */
    void noteActivity();
};

#endif