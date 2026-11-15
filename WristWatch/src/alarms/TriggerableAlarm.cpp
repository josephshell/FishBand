#include <alarms/TriggerableAlarm.h>
#include <time_helpers/TimeHelpers.h>

bool triggerAlreadyPassedToday(const tm& localTime, const Alarm& alarm);

TriggerableAlarm::TriggerableAlarm(const Alarm& alarm) : alarm(alarm)
{
    nextTriggerTime = 0;
}

bool TriggerableAlarm::shouldTrigger(time_t currentTime)
{
    if (nextTriggerTime == 0)
    {
        recalculateNextTriggerTime(currentTime);
    }
    return currentTime >= nextTriggerTime;
}

void TriggerableAlarm::recalculateNextTriggerTime(time_t currentTime)
{   
    tm localTime;
    TimeHelpers::getLocalTime(&localTime, currentTime);
    localTime.tm_hour = alarm.hour24();
    localTime.tm_min = alarm.minute;
    if (alarm.freq == ONCE || alarm.freq == EVDY)
    {
        if (triggerAlreadyPassedToday(localTime, alarm))
        {
            // The alarm time has already passed today, so set the next trigger time to tomorrow
            localTime.tm_mday += 1;
        }
    }
    if (alarm.freq == WKDY)
    {
        // Trigger is next possible time today or tomorrow, but only if that day is a work day
        if (localTime.tm_wday < 5) // If today is a work day (Su=0, Mo=1, Tu=2, We=3, Th=4)
        {
            if (triggerAlreadyPassedToday(localTime, alarm))
            {
                // The alarm time has already passed today, so set the next trigger time to tomorrow
                localTime.tm_mday += 1;
            }
        }
        else
        {
            // Today is not a work day, so set the next trigger time to tomorrow
            localTime.tm_mday += 1;
        }
    }
    if (alarm.freq == WKND)
    {
        // Trigger is next possible time today or tomorrow, but only if that day is a weekend day
        if (localTime.tm_wday == 5) // If today is a friday (Fr=5)
        {
            if (triggerAlreadyPassedToday(localTime, alarm))
            {
                // The alarm time has already passed today, so set the next trigger time to tomorrow (saturday)
                localTime.tm_mday += 1;
            }
        }
        else if (localTime.tm_wday == 6) // If today is a saturday (Sa=6)
        {
            if (triggerAlreadyPassedToday(localTime, alarm))
            {
                // The alarm time has already passed today, so set the next trigger time to next week on the first weekend day (friday)
                localTime.tm_mday += 6; // Move to next friday
            }
        }
        else
        {
            // Today is not a weekend day, so set the next trigger time to tomorrow
            localTime.tm_mday += 1;
        }
    }
    nextTriggerTime = mktime(&localTime);
}

time_t TriggerableAlarm::getNextTriggerTime()
{
    return nextTriggerTime;
}

bool triggerAlreadyPassedToday(const tm& localTime, const Alarm& alarm)
{
    return localTime.tm_hour > alarm.hour || (localTime.tm_hour == alarm.hour && localTime.tm_min >= alarm.minute);
}