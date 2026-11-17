#include <alarms/Alarms.h>
#include <alarms/AlarmManager.h>
#include <time_helpers/TimeHelpers.h>

#include <vector>

const long SECONDS_IN_A_DAY = 86400;

void AlarmWithTime::recalculateTriggerTime(time_t currentTime)
{
    tm currentLocalTime{};
    TimeHelpers::getLocalTime(&currentLocalTime, currentTime);
    tm triggerLocalTime{};
    TimeHelpers::getLocalTime(&triggerLocalTime, nextTriggerTime);
    switch (alarm.freq)
    {
    case EVDY:
        while (currentTime > nextTriggerTime)
        {
            nextTriggerTime += SECONDS_IN_A_DAY;
        }
        break;
    case WKDY:
        while (currentTime > nextTriggerTime)
        {
            nextTriggerTime += SECONDS_IN_A_DAY;
        }
        TimeHelpers::getLocalTime(&triggerLocalTime, nextTriggerTime);
        while (triggerLocalTime.tm_wday > 4) // after thursday
        {
            nextTriggerTime += SECONDS_IN_A_DAY;
            TimeHelpers::getLocalTime(&triggerLocalTime, nextTriggerTime);
        }
    case WKND:
        while (currentTime > nextTriggerTime)
        {
            nextTriggerTime += SECONDS_IN_A_DAY;
        }
        TimeHelpers::getLocalTime(&triggerLocalTime, nextTriggerTime);
        while (triggerLocalTime.tm_wday < 5) // before Friday
        {
            nextTriggerTime += SECONDS_IN_A_DAY;
            TimeHelpers::getLocalTime(&triggerLocalTime, nextTriggerTime);
        }
    case ONCE:
        nextTriggerTime = 0; // DO not retrigger
    default:
        break;
    }
}

time_t calculateTriggerTime(time_t currentTime, Alarm alarm)
{
    return 0;
}

AlarmManager::AlarmManager(TimeProvider *tp)
{
    this->tp = tp;
}

int AlarmManager::registerAlarm(Alarm alarm)
{
    alarms.push_back(alarm);
    timedAlarms.emplace_back(alarm, calculateTriggerTime(tp->getCurrentTime(), alarm));
    return 0;
}

std::vector<Alarm> AlarmManager::alarmsMustTrigger()
{
    std::vector<Alarm> toTrigger{};
    for (AlarmWithTime &alarm : timedAlarms)
    {
        if (alarm.nextTriggerTime < tp->getCurrentTime())
        {
            toTrigger.push_back(alarm.alarm);
        }
    }
    return toTrigger;
}

void AlarmManager::recalculateTriggers()
{
    time_t currentTime = tp->getCurrentTime();
    for (AlarmWithTime &alarm : timedAlarms)
    {
        if (alarm.alarm.freq != ONCE && alarm.nextTriggerTime < currentTime)
        {
            alarm.recalculateTriggerTime(tp->getCurrentTime());
        }
    }
}