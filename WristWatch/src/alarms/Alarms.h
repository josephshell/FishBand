#ifndef ALARMS_H
#define ALARMS_H

enum AlarmFrequency
{
	EVDY, // Every day
	WKDY, // Every weekday (Su-Th)
	WKND, // Every weekend day (Fr + Sa)
	ONCE  // One time
};

const char *getFrequencyStr(AlarmFrequency freq)
{
	switch (freq)
	{
	case EVDY:
		return "EVDY";
	case WKDY:
		return "WKDY";
	case WKND:
		return "WKND";
	case ONCE:
		return "ONCE";
	default:
		return "UH..";
	}
}

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
	uint8_t hour = 12;
	uint8_t minute;
	bool isMeridiemAm;
	AlarmFrequency freq = ONCE;

	void clear()
	{
		hour = 12;
		minute = 0;
		isMeridiemAm = true;
		freq = ONCE;
	}
};
#endif