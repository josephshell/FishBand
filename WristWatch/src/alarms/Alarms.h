#ifndef ALARMS_H
#define ALARMS_H

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

	void clear()
	{
		hour = 12;
		minute = 0;
		isMeridiemAm = false;
		freq = ONCE;
	}
};
#endif