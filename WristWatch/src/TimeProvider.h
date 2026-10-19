#ifndef TIME_PROVIDER_H
#define TIME_PROVIDER_H

#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <ctime>

class TimeProvider
{
public:
	virtual void initialize() = 0;

	virtual time_t getCurrentTime() = 0;

	virtual ~TimeProvider() = default;
};

class DS1307TimeProvider : public TimeProvider
{
private:
	RTC_DS1307 rtc;
	bool rtcFailure = false;

public:
	void initialize() override
	{
		if (!rtc.begin())
		{
			Serial.println("RTC begin failure");
			rtcFailure = true;
		}
	}

	time_t getCurrentTime() override
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
};

#endif