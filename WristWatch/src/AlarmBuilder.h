#ifndef ALARM_BUILDER_H
#define ALARM_BUILDER_H

#include <Arduino.h>
#include <stdio.h> // For snprintf

enum AlarmFrequency
{
	EVDY, // Every day
	WKDY, // Every weekday (Su-Th)
	WKND, // Every weekend day (Fr + Sa)
	ONCE  // One time
};

char *getFrequencyStr(AlarmFrequency freq)
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

enum CursorLocation
{
	HOUR,
	MINUTE,
	MERIDIEM,
	FREQUENCY,
	FINISH
};

struct Alarm
{
	uint8_t hour;
	uint8_t minute;
	bool isMeridiemAm;
	AlarmFrequency freq;
};

class AlarmBuilder
{
private:
	uint8_t hour = 12;
	uint8_t minute;
	bool isMeridiemAm;
	AlarmFrequency freq;
	CursorLocation cursor;

public:
	AlarmBuilder() = default;

	CursorLocation getCursorLocation()
	{
		return cursor;
	}

	uint8_t getHour()
	{
		return hour;
	}

	AlarmFrequency getFrequency()
	{
		return freq;
	}

	bool isAm()
	{
		return isMeridiemAm;
	}

	uint8_t getMinute()
	{
		return minute;
	}

	void cursorBackward()
	{
		if (cursor == HOUR)
		{
			cursor = FINISH;
		}
		else
		{
			cursor = static_cast<CursorLocation>(cursor - 1);
		}
		Serial.print("Cursor is: ");
		Serial.println(cursor);
	}

	void cursorForward()
	{
		cursor = static_cast<CursorLocation>(cursor + 1);
		if (cursor > FINISH)
		{
			cursor = HOUR;
		}
		Serial.print("Cursor is: ");
		Serial.println(cursor);
	}

	/**
	 * Modifies the element at the cursor by modification value. Returns 0 if alarm is finished, 1 if not finished
	 *
	 * modification: positive or negative value to modify the element at the cursor.
	 * For MERIDIEM, any non-zero value will toggle it.
	 * For FREQUENCY, positive values will raise frequency one level, negative values reduce frequency one level.
	 * For FINISH, positive values will finish the alarm returning 0, negative values will do nothing.
	 */
	int modifySelected(int8_t modification)
	{
		switch (cursor)
		{
		case HOUR:
			Serial.print("HOUR + ");
			Serial.println(modification);
			modifyHour(modification);
			break;
		case MINUTE:
			Serial.print("MINUTE + ");
			Serial.println(modification);
			modifyMinute(modification);
			break;
		case MERIDIEM:
			Serial.println("MERIDIEM TOGGLE");
			isMeridiemAm = !isMeridiemAm;
			break;
		case FREQUENCY:
			Serial.print("FREQ + ");
			Serial.println(modification);
			if (modification > 0)
			{
				frequencyUp();
			}
			else
			{
				frequencyDown();
			}
			break;
		case FINISH:
			if (modification > 0)
			{
				Serial.println("FINISH");
				return 0;
			}
			break;
		default:
			if (Serial)
			{
				Serial.println("Modification failure, cursor at invalid location. Resetting cursor");
				cursor = HOUR;
			}
			break;
		}
		return 1;
	}

	void modifyHour(int8_t modification)
	{
		if (modification >= 0)
		{
			hour += modification;
			while (hour > 12)
			{
				hour -= 12;
			}
		}
		else
		{
			if (-modification >= hour)
			{
				hour = 12 - (-modification - hour) % 12;
			}
			else
			{
				hour += modification;
			}
		}
	}

	void modifyMinute(int8_t modification)
	{
		if (modification >= 0)
		{
			minute += modification;
			while (minute >= 60)
			{
				minute -= 60;
			}
		}
		else
		{
			if (-modification >= minute)
			{
				minute = 60 - (-modification - minute) % 60;
				if (minute == 60)
				{
					minute = 0;
				}
			}
			else
			{
				minute += modification;
			}
		}
	}

	void toggleAmPm()
	{
		isMeridiemAm = !isMeridiemAm;
	}

	void frequencyUp()
	{
		if (freq == EVDY)
		{ // if at highest freq, go to bottom
			freq = ONCE;
		}
		else
		{
			freq = static_cast<AlarmFrequency>(freq - 1);
		}
	}

	void frequencyDown()
	{
		if (freq == ONCE)
		{ // if at lowest freq, go to top
			freq = EVDY;
		}
		else
		{
			freq = static_cast<AlarmFrequency>(freq + 1);
		}
	}

	char *alarmToString(char *outputBuffer, size_t bufferSize)
	{
		snprintf(outputBuffer, bufferSize, "%02d:%02d %s %s", hour, minute, isMeridiemAm ? "AM" : "PM", getFrequencyStr(freq));
		return outputBuffer;
	}

	Alarm buildAlarm()
	{
		Alarm alarm;
		alarm.hour = hour;
		alarm.minute = minute;
		alarm.isMeridiemAm = isMeridiemAm;
		alarm.freq = freq;
		return alarm;
	}
};
#endif