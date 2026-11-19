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
        while (currentTime >= nextTriggerTime)
        {
            nextTriggerTime += SECONDS_IN_A_DAY;
        }
        break;
    case WKDY:
        while (currentTime >= nextTriggerTime)
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
        while (currentTime >= nextTriggerTime)
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
        // DO not retrigger
        enabled = false;
        nextTriggerTime = 0;
    default:
        break;
    }
}

void moveToNextValidDay(tm &alarmTrigger, const Alarm& alarm)
{
    //TODO: Calculate next valid day based on Frequency and current day setting
    alarmTrigger.tm_mday += 1;
    // Normalizes time struct
    mktime(&alarmTrigger);
}

time_t calculateTriggerTime(time_t currentTime, Alarm alarm)
{
    tm currentTimeAsLocal{};
    TimeHelpers::getLocalTime(&currentTimeAsLocal, currentTime);
    // Duplicate current datetime for date, then fix time
    tm nextTriggerLocalTime = currentTimeAsLocal;
    int alarmHour;
    if (alarm.isMeridiemAm) {
        alarmHour = alarm.hour == 12 ? 0 : alarm.hour;
    } else
    {
        alarmHour = alarm.hour == 12 ? 12 : alarm.hour + 12;
    }
    nextTriggerLocalTime.tm_hour = alarmHour;
    nextTriggerLocalTime.tm_min = alarm.minute;
    time_t alarmTriggerTimestamp = mktime(&nextTriggerLocalTime);
    if (alarmTriggerTimestamp < currentTime)
    {
        moveToNextValidDay(nextTriggerLocalTime, alarm);
        alarmTriggerTimestamp = mktime(&nextTriggerLocalTime);
    }
    return alarmTriggerTimestamp;
}

AlarmManager::AlarmManager(TimeProvider *tp)
{
    this->tp = tp;
}


time_t AlarmManager::registerAlarm(Alarm alarm)
{
    alarms.push_back(alarm);
    time_t triggerTime = calculateTriggerTime(tp->getCurrentTime(), alarm);
    timedAlarms.emplace_back(alarm, triggerTime);
    return triggerTime;
}

std::vector<Alarm> AlarmManager::alarmsMustTrigger()
{
    std::vector<Alarm> toTrigger{};
    for (AlarmWithTime &alarm : timedAlarms)
    {
        if (alarm.enabled && alarm.nextTriggerTime <= tp->getCurrentTime())
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
        if (alarm.nextTriggerTime <= currentTime)
        {
            alarm.recalculateTriggerTime(tp->getCurrentTime());
        }
    }
}