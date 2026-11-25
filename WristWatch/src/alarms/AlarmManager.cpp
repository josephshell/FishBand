#include <alarms/Alarms.h>
#include <alarms/AlarmManager.h>
#include <time_helpers/TimeHelpers.h>

#include <vector>

const long SECONDS_IN_A_DAY = 86400;

bool isValidTriggerDay(tm &alarmTrigger, const AlarmFrequency &freq)
{
    switch (freq)
    {
    case AlarmFrequency::EVDY:
        return true;
    case AlarmFrequency::ONCE:
        return true;
    case AlarmFrequency::WKDY:
        switch (alarmTrigger.tm_wday)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4: // Su, Mo, Tu, We, Th
            return true;
        default:
            return false;
        }
    case AlarmFrequency::WKND:
        return alarmTrigger.tm_wday == 5 || alarmTrigger.tm_wday == 6; // Fr, Sa
    default:
        return true;
    }
}

void moveToNextValidDay(tm &alarmTrigger, const Alarm &alarm)
{
    do
    {
        alarmTrigger.tm_mday += 1;
        // Normalizes time struct
        TimeHelpers::tmAsTimestamp(&alarmTrigger);
    } while (!isValidTriggerDay(alarmTrigger, alarm.freq));
}

void AlarmWithTime::recalculateTriggerTime(time_t currentTime)
{
    tm currentLocalTime{};
    TimeHelpers::timestampAsTm(&currentLocalTime, currentTime);
    tm triggerLocalTime{};
    TimeHelpers::timestampAsTm(&triggerLocalTime, nextTriggerTime);

    while (currentTime >= nextTriggerTime)
    {
        nextTriggerTime += SECONDS_IN_A_DAY;
    }
    TimeHelpers::timestampAsTm(&triggerLocalTime, nextTriggerTime);
    if (alarm.freq == ONCE)
    {
        enabled = false;
        nextTriggerTime = 0;
    }
    else
    {
        moveToNextValidDay(triggerLocalTime, alarm);
    }
}

time_t calculateTriggerTime(time_t currentTime, Alarm alarm)
{
    tm currentTimeAsLocal{};
    TimeHelpers::timestampAsTm(&currentTimeAsLocal, currentTime);
    // Duplicate current datetime for date, then fix time
    tm nextTriggerLocalTime = currentTimeAsLocal;
    int alarmHour;
    if (alarm.isMeridiemAm)
    {
        alarmHour = alarm.hour == 12 ? 0 : alarm.hour;
    }
    else
    {
        alarmHour = alarm.hour == 12 ? 12 : alarm.hour + 12;
    }
    nextTriggerLocalTime.tm_hour = alarmHour;
    nextTriggerLocalTime.tm_min = alarm.minute;
    nextTriggerLocalTime.tm_sec = 0;
    time_t alarmTriggerTimestamp = TimeHelpers::tmAsTimestamp(&nextTriggerLocalTime);
    if (alarmTriggerTimestamp < currentTime || !isValidTriggerDay(nextTriggerLocalTime, alarm.freq))
    {
        moveToNextValidDay(nextTriggerLocalTime, alarm);
        alarmTriggerTimestamp = TimeHelpers::tmAsTimestamp(&nextTriggerLocalTime);
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

time_t AlarmManager::nextTriggerTimestamp()
{
    bool triggerFound = false;
    time_t minimum = 0;
    for (AlarmWithTime &t : timedAlarms)
    {
        if (t.enabled)
        {
            if (!triggerFound)
            {
                minimum = t.nextTriggerTime;
                triggerFound = true;
            }
            else if (t.nextTriggerTime < minimum)
            {
                minimum = t.nextTriggerTime;
            }
        }
    }
    return minimum;
}