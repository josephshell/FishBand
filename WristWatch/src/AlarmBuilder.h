#ifndef ALARM_BUILDER_H
#define ALARM_BUILDER_H

#include <Arduino.h>

enum AlarmFrequency {
  EVDY, // Every day
  WKDY, // Every weekday (Su-Th)
  WKND, // Every weekend day (Fr + Sa)
  ONCE  // One time
};

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

  void cursorForward() {
    if (++cursor > FINISH) {
      cursor = HOUR;
    }
  }

  void modifySelected(int8_t modification) {
    switch (cursor) {
      case HOUR:
        // modify hour
        break;
      case MINUTE:
        // modify minute
        break;
      case MERIDIEM:
        // modify AM/PM
        break;
      case FREQUENCY:
        // modify frequency
        break;
      default:
        if (Serial) {
          Serial.println("Modification failure, cursor at invalid location. Resetting cursor");
          cursor = 0;
        }
        break;
    }
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
};
#endif