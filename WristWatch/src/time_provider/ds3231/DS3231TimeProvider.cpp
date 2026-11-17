#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>

#include <time_provider/ds3231/DS3231TimeProvider.h>

void DS3231TimeProvider::initialize()
{
    if (!rtc.begin())
    {
        Serial.println("RTC begin failure");
        rtcFailure = true;
    }
}

time_t DS3231TimeProvider::getCurrentTime()
{
    if (!rtcFailure)
    {
        return rtc.now().unixtime();
    }
    else
    {
        return 0;
    }
}

void DS3231TimeProvider::setCurrentTime(time_t newTime)
{
    if (!rtcFailure)
    {
        rtc.adjust(DateTime(newTime));
    }
}