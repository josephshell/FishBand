#ifndef ALARM_MANAGER_H
#define ALARM_MANAGER_H

#include <alarms/Alarms.h>
#include <time_provider/TimeProvider.h>

#include <vector>
#include <ctime>

/**
 *
 * Next trigger time of 0 means alarm was a once only alarm and has already triggered.
 */
struct AlarmWithTime
{
    Alarm alarm;
    time_t nextTriggerTime;
    bool enabled;

    AlarmWithTime(Alarm alarm, time_t nextTriggerTime, bool enabled = true)
    {
        this->alarm = alarm;
        this->nextTriggerTime = nextTriggerTime;
        this->enabled = enabled;
    }
    void recalculateTriggerTime(time_t currentTime);
};

class AlarmManager
{
private:
    std::vector<Alarm> alarms;
    std::vector<AlarmWithTime> timedAlarms;
    TimeProvider *tp;

public:
    AlarmManager(TimeProvider *tp);
    /**
     * @brief Registers an alarm to be managed
     *
     * @returns the next trigger time for the alarm, or 0 if failure
     */
    time_t registerAlarm(Alarm alarm);

    /**
     * @brief Checks all registered alarms for those that must trigger and returns them.
     *
     * @returns vector of all alarms that must trigger
     */
    std::vector<Alarm> alarmsMustTrigger();

    /**
     * @brief Recalculates triggers for alarms. For use when triggered alarms have been dealt with.
     */
    void recalculateTriggers();

    /**
     * @brief Gets the nearest triggering timestamp
     *
     * @returns a timestamp for the closest alarm, or 0 if none are triggerable
     */
    time_t nextTriggerTimestamp();
};

#endif