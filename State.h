#ifndef STATE_H
#define STATE_H

#include <Arduino.h>

/**
 * The general state of the watch. Represents a major shift in
 * functionality. Examples of well-defined states: Displaying
 * the time, creating a timer, viewing a timer, etc.
*/
enum State {
  DISPLAY_TIME,
  MAIN_MENU,
  ALARMS
};

#endif