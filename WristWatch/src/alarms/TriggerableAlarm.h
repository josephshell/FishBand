#ifndef TRIGGERABLE_ALARM_H
#define TRIGGERABLE_ALARM_H

#include <ctime>

#include <alarms/Alarms.h>

class TriggerableAlarm 
{
    private:
        Alarm alarm;
        time_t nextTriggerTime;
    public:
        TriggerableAlarm(const Alarm& alarm);

        bool shouldTrigger(time_t currentTime);

        void recalculateNextTriggerTime(time_t currentTime);

        time_t getNextTriggerTime();

        Alarm *getAlarm()
        {
            return &alarm;
        }

        const char *toString(char *outputBuffer, size_t bufferSize)
        {
            return alarm.toString(outputBuffer, bufferSize);
        }
};

#endif