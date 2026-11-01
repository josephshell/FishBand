#include <oled_display/OledDisplayFunctions.h>
#include <oled_display/OledHelpers.h>

// 14 px monospaced font
const uint8_t *font = u8g2_font_chargen_92_mf;

// 10 px monospaced font
const uint8_t *fontButtons = u8g2_font_spleen8x16_mf;

char hourBuffer[3];
char minuteBuffer[3];
char secondBuffer[3];

char yearBuffer[3];
char monthBuffer[3];
char dayBuffer[3];

char timeBuffer[9];
char dateBuffer[9];

const int leftMarginPx = 5;
const int rightMarginPx = 5;
const int bottomMarginPx = 5;
const int bottomMarginOffsetPx = oled::height - bottomMarginPx;
const int wordHeightPx = 16;
const int verticalSpacingPx = 2;
const int column2XOffsetPx = 20;

void OledDisplayFunctions::displaySettingTime(const tm *localTime, TimeHelpers::TimeCursor timeCursor, U8G2_SH1106_128X64_NONAME_1_HW_I2C *u8g2)
{
    snprintf(hourBuffer, sizeof hourBuffer, "%.2d", localTime->tm_hour);
    snprintf(minuteBuffer, sizeof minuteBuffer, "%.2d", localTime->tm_min);
    snprintf(secondBuffer, sizeof secondBuffer, "%.2d", localTime->tm_sec);

    snprintf(yearBuffer, sizeof yearBuffer, "%.2d", localTime->tm_year - 100);
    snprintf(monthBuffer, sizeof monthBuffer, "%.2d", localTime->tm_mon + 1);
    snprintf(dayBuffer, sizeof dayBuffer, "%.2d", localTime->tm_mday);

    snprintf(timeBuffer, sizeof timeBuffer, "%.2d:%.2d:%.2d", localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
    snprintf(dateBuffer, sizeof dateBuffer, "%.2d/%.2d/%.2d", localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_year - 100);

    u8g2->firstPage();
    do
    {
        u8g2->setDrawColor(1);
        u8g2->setFont(font);

        oled::drawStrCentered(wordHeightPx, timeBuffer, u8g2);
        oled::drawStrCentered(wordHeightPx * 2 + verticalSpacingPx, dateBuffer, u8g2);

        u8g2->setFont(fontButtons);
        u8g2->drawStr(leftMarginPx, bottomMarginOffsetPx, "CANCEL");
        u8g2->drawStr(oled::width - rightMarginPx - u8g2->getStrWidth("DONE"), bottomMarginOffsetPx, "DONE");

    } while (u8g2->nextPage());
}