#ifndef OLED_DISPLAY_FUNCTIONS
#define OLED_DISPLAY_FUNCTIONS

#include <U8g2lib.h>

#include <time_helpers/TimeHelpers.h>
#include <ctime>

namespace OledDisplayFunctions
{
    void displaySettingTime(const tm *localTime, TimeHelpers::TimeCursor timeCursor, U8G2_SH1106_128X64_NONAME_1_HW_I2C* u8g2);
}

#endif