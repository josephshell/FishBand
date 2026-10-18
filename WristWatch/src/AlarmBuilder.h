#ifndef ALARM_BUILDER_H
#define ALARM_BUILDER_H

#include <Arduino.h>
#include <stdio.h> // For snprintf

enum AlarmFrequency {
  EVDY, // Every day
  WKDY, // Every weekday (Su-Th)
  WKND, // Every weekend day (Fr + Sa)
  ONCE  // One time
};

char *getFrequencyStr(AlarmFrequency freq) {
  switch (freq) {
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

enum CursorLocation {
  HOUR,
  MINUTE,
  MERIDIEM,
  FREQUENCY,
  FINISH
};

class AlarmBuilder {
private:
  uint8_t hour;
  uint8_t minute;
  bool isMeridiemAm;
  AlarmFrequency freq;
  uint8_t cursor;
public:
  AlarmBuilder() = default;

  uint8_t getCursor() {
    return cursor;
  }
  
  uint8_t getHour() {
    return hour;
  }

  AlarmFrequency getFrequency() {
    return freq;
  }

  bool isAm() {
    return isMeridiemAm;
  }

  uint8_t getMinute() {
    return minute;
  }

  void cursorBackward() {
    if (cursor == HOUR) {
      cursor = FINISH;
    } else {
	  --cursor;
	}
  }

  void cursorForward() {
    if (++cursor > FINISH) {
      cursor = HOUR;
    }
  }

  /**
   * Modifies the element at the cursor by modification value. Returns 0 if alarm is finished, 1 if not finished
   */
  int modifySelected(int8_t modification) {
    switch (cursor) {
		case HOUR:
			Serial.print("HOUR + ");
			Serial.println(modification);
			break;
		case MINUTE:
			Serial.print("MINUTE + ");
			Serial.println(modification);
			break;
		case MERIDIEM:
			Serial.println("MERIDIEM TOGGLE");
			break;
		case FREQUENCY:
			Serial.print("FREQ + ");
			Serial.println(modification);
			break;
		case FINISH:
			if (modification > 0) {
				Serial.println("FINISH");
				return 0;
			}
			break;
		default:
			if (Serial) {
				Serial.println("Modification failure, cursor at invalid location. Resetting cursor");
				cursor = 0;
			}
			break;
    }
	return 1;
  }

  void modifyHour(int8_t modification) {
    if (modification >= 0) {
      hour += modification;
      if (hour > 12) {
        hour -= 12;
      }
    } else {
      // handle negative
    }
  }

  void modifyMinute(int8_t modification) {
    if (modification >= 0) {
      minute += modification;
      if (minute >= 60) {
        minute -= 60;
      }
    } else {
      // handle negative
    }
  }

  void toggleAmPm() {
    isMeridiemAm = !isMeridiemAm;
  }

  void frequencyUp() {
    if (freq == EVDY) { // if at highest freq, go to bottom
      freq = ONCE;
    } else {
      freq = static_cast<AlarmFrequency>(freq - 1);
    }
  }

  void frequencyDown() {
    if (freq == ONCE) { // if at lowest freq, go to top
      freq = EVDY;
    } else {
      freq = static_cast<AlarmFrequency>(freq + 1);
    }
  }

  char* debugString() {
	static char buffer[32];
	snprintf(buffer, sizeof(buffer), "%02d:%02d %s %s", hour, minute, isMeridiemAm ? "AM" : "PM", getFrequencyStr(freq));
	return buffer;
  }
};
#endif