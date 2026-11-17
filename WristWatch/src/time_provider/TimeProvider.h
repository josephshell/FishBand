#ifndef TIME_PROVIDER_H
#define TIME_PROVIDER_H

#include <ctime>

class TimeProvider
{
public:
	virtual void initialize() = 0;
	virtual time_t getCurrentTime() = 0;
	virtual void setCurrentTime(time_t newTime) = 0;
	virtual ~TimeProvider() = default;
};

#endif