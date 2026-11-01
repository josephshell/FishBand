#ifndef OLED_HELPERS_H
#define OLED_HELPERS_H

#include <U8g2lib.h>

namespace oled {
    const uint8_t width = 128;
    const uint8_t height = 64;

    void drawStrCentered(uint8_t y, const char *str, U8G2_SH1106_128X64_NONAME_1_HW_I2C *u8g2);
}

#endif