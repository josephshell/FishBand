#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Arduino.h>
#include <Display.h>
#include <Menu.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <WatchFaces.h>
#include <stdio.h> // For snprintf

// 16 px monospaced font
const uint8_t *font_time = u8g2_font_inr16_mn;
// 10 px monospaced font
const uint8_t *font_day_of_the_week = u8g2_font_spleen8x16_mf;
// 8 px monospaced font
const uint8_t *font_date = u8g2_font_6x10_mn;
// 14 px monospaced font
const uint8_t *font_menu = u8g2_font_chargen_92_mf;
// 10 px monospaced font
const uint8_t *font_alarm_builder = u8g2_font_spleen8x16_mf;

class OledDisplay : public Display
{
private:
	U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2;
	const uint8_t width = 128;
	const uint8_t height = 64;

	void drawStrCentered(uint8_t y, const char *str)
	{
		uint8_t strWidth = u8g2.getStrWidth(str);
		if (strWidth > width)
		{
			u8g2.drawStr(0, y, str);
		}
		else
		{
			uint8_t x = (width - strWidth) / 2;
			u8g2.drawStr(x, y, str);
		}
	}

public:
	OledDisplay() : u8g2(U8G2_R0, U8X8_PIN_NONE) {}

	void initialize() override
	{
		u8g2.begin();
	}

	void displayDateTime(std::time_t &time) override
	{
		std::tm *localTime = std::localtime(&time);
		char dayBuffer[4];
		char dateBuffer[17];
		char timeBuffer[17];
		std::strftime(dayBuffer, sizeof(dayBuffer), "%a", localTime);
		std::strftime(dateBuffer, sizeof(dateBuffer), "%m/%d", localTime);
		std::strftime(timeBuffer, sizeof(timeBuffer), "%I:%M %p", localTime);
		u8g2.firstPage();
		do
		{
			u8g2.drawXBMP(0, 0, 128, 64, epd_bitmap_WatchFace_cornergecko);
			u8g2.setFontMode(1);
			u8g2.setFont(font_day_of_the_week);
			u8g2.drawStr(0, 15, dayBuffer);
			u8g2.setFont(font_date);
			u8g2.drawStr(0, 25, dateBuffer);
			u8g2.setFont(font_time);
			u8g2.drawStr(32, 42, timeBuffer);
		} while (u8g2.nextPage());
	}

	void displayMenu(Menu &menu) override
	{
		// if option count < 3, don't display
		const char *current = menu.textAtCursorOffset(0);
		const char *previous = menu.textAtCursorOffset(-1);
		const char *next = menu.textAtCursorOffset(1);
		uint8_t optionCount = menu.getOptionCount();
		u8g2.firstPage();
		do
		{
			u8g2.setDrawColor(1);
			u8g2.drawRBox(2, 22, 124, 22, 7);

			u8g2.setFont(font_menu);

			// Black for highlighted option
			u8g2.setDrawColor(0);
			// Center Option
			drawStrCentered(40, current);
			// Blue for non highlighted
			u8g2.setDrawColor(1);
			if (optionCount > 1)
			{
				// Bottom Option
				drawStrCentered(60, next);
			}
			if (optionCount > 2)
			{
				// Top Option
				drawStrCentered(18, previous);
			}
		} while (u8g2.nextPage());
	}

	void displayAlarm(Alarm &alarm, AlarmCursorLocation cursor) override
	{
		uint8_t leftMarginPx = 5;
		uint8_t rightMarginPx = 5;
		uint8_t timeTopOffset = 30;
		uint8_t meridiemExSpacing = 2;
		uint8_t buttonWidth = 45;
		uint8_t buttonX = width - (buttonWidth / 2) - rightMarginPx;
		uint8_t buttonY = 55;
		uint8_t buttonHPadding = 3;

		char timeBuffer[6];
		uint8_t bufferLen = sizeof timeBuffer;
		snprintf(timeBuffer, bufferLen, "%.2d:%.2d", alarm.hour, alarm.minute);
		const char *meridiem = alarm.isMeridiemAm ? "AM" : "PM";
		u8g2.firstPage();
		do
		{
			u8g2.setDrawColor(1);
			u8g2.setFont(font_time);

			u8g2.drawStr(leftMarginPx, timeTopOffset, timeBuffer);
			uint8_t timeStrWidth = u8g2.getStrWidth(timeBuffer);
			uint8_t hourWidth = u8g2.getStrWidth("00");
			if (cursor == HOUR)
			{
				u8g2.drawLine(leftMarginPx, timeTopOffset + 2, leftMarginPx + hourWidth, timeTopOffset + 2);
			}
			else if (cursor == MINUTE)
			{
				uint8_t minuteX1 = leftMarginPx + hourWidth + u8g2.getStrWidth(":");
				uint8_t minuteWidth = u8g2.getStrWidth("00");
				u8g2.drawLine(minuteX1, timeTopOffset + 2, leftMarginPx + timeStrWidth, timeTopOffset + 2);
			}

			u8g2.setFont(font_alarm_builder);

			u8g2.drawStr(leftMarginPx + timeStrWidth + meridiemExSpacing, timeTopOffset, meridiem);
			if (cursor == MERIDIEM)
			{
				u8g2.drawLine(leftMarginPx + timeStrWidth + meridiemExSpacing, timeTopOffset + 2, leftMarginPx + timeStrWidth + meridiemExSpacing + u8g2.getStrWidth(meridiem), timeTopOffset + 2);
			}
			u8g2.drawStr(leftMarginPx, buttonY, getFrequencyStr(alarm.freq));
			if (cursor == FREQUENCY)
			{
				u8g2.drawLine(leftMarginPx, buttonY + 2, leftMarginPx + u8g2.getStrWidth(getFrequencyStr(alarm.freq)), buttonY + 2);
			}
			if (cursor == FINISH)
			{
				u8g2.drawButtonUTF8(buttonX, buttonY, U8G2_BTN_HCENTER | U8G2_BTN_BW3 | U8G2_BTN_INV, buttonWidth, 0, buttonHPadding, "DONE");
			}
			else
			{
				u8g2.drawButtonUTF8(buttonX, buttonY, U8G2_BTN_HCENTER | U8G2_BTN_BW3, buttonWidth, 0, buttonHPadding, "DONE");
			}
		} while (u8g2.nextPage());
	}

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

	void displayImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap) override
	{
		u8g2.firstPage();
		do
		{
			u8g2.drawXBMP(x, y, w, h, bitmap);
		} while (u8g2.nextPage());
	}

	void runTest() override
	{
		char previous[] = "Return";
		char current[] = "Alarms";
		char next[] = "Show funny";
		u8g2.firstPage();
		do
		{
			u8g2.setDrawColor(1);
			u8g2.drawRBox(5, 22, 114, 20, 5);

			u8g2.setFont(font_menu);

			// Black for highlighted option
			u8g2.setDrawColor(0);
			// Center Option
			drawStrCentered(40, current);
			// Blue for non highlighted
			u8g2.setDrawColor(1);
			// Top Option
			drawStrCentered(18, previous);
			// Bottom Option
			drawStrCentered(60, next);
		} while (u8g2.nextPage());
	}

	void clear() override
	{
		u8g2.clear();
	}
};
#endif