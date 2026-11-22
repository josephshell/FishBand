#include <sleep/IdleTimer.h>

namespace
{
    void internalNoteActivity(time_t &activityTrackerVar, time_t time)
    {
        activityTrackerVar = time;
    }
}

bool IdleTimer::isUserIdle()
{
    if (tp)
    {
        time_t time = tp->getCurrentTime();
        if (lastActivityTime == 0)
            internalNoteActivity(lastActivityTime, time);
        return time - lastActivityTime >= idleTimeout;
    }
    else
    {
        return false; // if TimeProvider is null, never idle
    }
}

void IdleTimer::noteActivity()
{
    if (tp)
        internalNoteActivity(lastActivityTime, tp->getCurrentTime());
}