#ifndef DS3231_TIME_PROVIDER_H
#define DS3231_TIME_PROVIDER_H

#include <time_provider/TimeProvider.h>

#include <RTClib.h>

class DS3231TimeProvider : public TimeProvider
{
private:
    RTC_DS3231 rtc;
    bool rtcFailure = false;
public:
    void initialize() override;
    time_t getCurrentTime() override;
    void setCurrentTime(time_t newTime) override;
};

#endif