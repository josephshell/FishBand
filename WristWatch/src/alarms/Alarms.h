#ifndef ALARMS_H
#define ALARMS_H

#include <stdio.h> // For snprintf

enum AlarmFrequency
{
	EVDY, // Every day
	WKDY, // Every weekday (Su-Th)
	WKND, // Every weekend day (Fr + Sa)
	ONCE  // One time
};

const char *getFrequencyStr(AlarmFrequency freq);

enum AlarmCursorLocation
{
	HOUR,
	MINUTE,
	MERIDIEM,
	FREQUENCY,
	FINISH
};

struct Alarm
{
	int hour = 12;
	int minute = 0;
	bool isMeridiemAm = false;
	AlarmFrequency freq = ONCE;

	Alarm() {};
	Alarm(int hour, int minute, bool isAM, AlarmFrequency freq) : hour(hour), minute(minute), isMeridiemAm(isAM), freq(freq) {};

	void clear()
	{
		hour = 12;
		minute = 0;
		isMeridiemAm = false;
		freq = ONCE;
	}

	char *toString(char *outputBuffer, size_t bufferSize)
	{
		snprintf(outputBuffer, bufferSize, "%02d:%02d %s %s", hour, minute, isMeridiemAm ? "AM" : "PM", getFrequencyStr(freq));
		return outputBuffer;
	}
};
#endif