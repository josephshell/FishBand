#include <oled_display/OledHelpers.h>

void oled::drawStrCentered(uint8_t y, const char *str, U8G2_SH1106_128X64_NONAME_1_HW_I2C *u8g2)
{
    uint8_t strWidth = u8g2->getStrWidth(str);
    if (strWidth > width)
    {
        u8g2->drawStr(0, y, str);
    }
    else
    {
        uint8_t x = (width - strWidth) / 2;
        u8g2->drawStr(x, y, str);
    }
}