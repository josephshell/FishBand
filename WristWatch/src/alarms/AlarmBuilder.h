#ifndef ALARM_BUILDER_H
#define ALARM_BUILDER_H

#include <Arduino.h>
#include <alarms/Alarms.h>
#include <stdio.h> // For snprintf


class AlarmBuilder
{
private:
	Alarm alarm;
	AlarmCursorLocation cursor;

public:
	AlarmBuilder() = default;

	AlarmCursorLocation getCursorLocation()
	{
		return cursor;
	}

	int getHour()
	{
		return alarm.hour;
	}

	AlarmFrequency getFrequency()
	{
		return alarm.freq;
	}

	bool isAm()
	{
		return alarm.isMeridiemAm;
	}

	uint8_t getMinute()
	{
		return alarm.minute;
	}

	void cursorBackward()
	{
		if (cursor == HOUR)
		{
			cursor = FINISH;
		}
		else
		{
			cursor = static_cast<AlarmCursorLocation>(cursor - 1);
		}
		Serial.print("Cursor is: ");
		Serial.println(cursor);
	}

	void cursorForward()
	{
		cursor = static_cast<AlarmCursorLocation>(cursor + 1);
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
			alarm.isMeridiemAm = !alarm.isMeridiemAm;
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
		//TODO: Simplify math now that hour and minute are signed
		int hour = alarm.hour;
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
		alarm.hour = hour;
	}

	void modifyMinute(int8_t modification)
	{
		int minute = alarm.minute;
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
		alarm.minute = minute;
	}

	void toggleAmPm()
	{
		alarm.isMeridiemAm = !alarm.isMeridiemAm;
	}

	void frequencyUp()
	{
		if (alarm.freq == EVDY)
		{ // if at highest freq, go to bottom
			alarm.freq = ONCE;
		}
		else
		{
			alarm.freq = static_cast<AlarmFrequency>(alarm.freq - 1);
		}
	}

	void frequencyDown()
	{
		if (alarm.freq == ONCE)
		{ // if at lowest freq, go to top
			alarm.freq = EVDY;
		}
		else
		{
			alarm.freq = static_cast<AlarmFrequency>(alarm.freq + 1);
		}
	}

	char *alarmToString(char *outputBuffer, size_t bufferSize)
	{
		snprintf(outputBuffer, bufferSize, "%02d:%02d %s %s", alarm.hour, alarm.minute, alarm.isMeridiemAm ? "AM" : "PM", getFrequencyStr(alarm.freq));
		return outputBuffer;
	}

	Alarm buildAlarm(Alarm& outputAlarm)
	{
		outputAlarm.hour = alarm.hour;
		outputAlarm.minute = alarm.minute;
		outputAlarm.isMeridiemAm = alarm.isMeridiemAm;
		outputAlarm.freq = alarm.freq;
		return outputAlarm;
	}
};
#endif