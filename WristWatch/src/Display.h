#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <ctime>
#include <alarms/AlarmBuilder.h>

class Display
{
public:
	virtual void initialize() = 0;

	virtual void mainDisplay(std::time_t &time, int8_t batteryLevel) = 0;

	virtual void displayMenu(Menu &menu) = 0;

	virtual void displayAlarm(Alarm &alarm, AlarmCursorLocation cursor) = 0;

	virtual void displayImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap) = 0;

	virtual void runTest() = 0;

	virtual void clear() = 0;

	virtual ~Display() = default;
};

#endif