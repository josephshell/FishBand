#include <oled_display/OledDisplayFunctions.h>
#include <oled_display/OledHelpers.h>

using namespace TimeHelpers;

int getXOfDoneButton(U8G2_SH1106_128X64_NONAME_1_HW_I2C *u8g2);
void applyHighlight(TimeCursor timeCursor, U8G2_SH1106_128X64_NONAME_1_HW_I2C *u8g2);

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
const int topMarginPx = 5;
const int wordHeightPx = 14;
const int verticalSpacingPx = 4;
const int bottomMarginOffsetPx = oled::height - bottomMarginPx;

const int timeAndDateXValue = 16;
const int timeYValue = wordHeightPx + topMarginPx;
const int dateYValue = timeYValue + verticalSpacingPx + wordHeightPx;
const int buttonYValue = bottomMarginOffsetPx;
const int cancelButtonX = leftMarginPx;

void OledDisplayFunctions::displaySettingTime(const tm *localTime, TimeCursor timeCursor, U8G2_SH1106_128X64_NONAME_1_HW_I2C *u8g2)
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
        
        u8g2->drawStr(timeAndDateXValue, timeYValue, timeBuffer);
        u8g2->drawStr(timeAndDateXValue, dateYValue, dateBuffer);

        u8g2->setFont(fontButtons);
        u8g2->drawStr(leftMarginPx, buttonYValue, "CANCEL");
        u8g2->drawStr(getXOfDoneButton(u8g2), buttonYValue, "DONE");

        applyHighlight(timeCursor, u8g2);

    } while (u8g2->nextPage());
}

/**
 * WARNING: CHANGES FONT
 */
int getXOfDoneButton(U8G2_SH1106_128X64_NONAME_1_HW_I2C *u8g2)
{
    u8g2->setFont(fontButtons);
    return oled::width - rightMarginPx - u8g2->getStrWidth("DONE");
}

void applyHighlight(TimeCursor timeCursor, U8G2_SH1106_128X64_NONAME_1_HW_I2C *u8g2)
{
    u8g2->setFont(font);
    int charWidth = u8g2->getStrWidth("0");
    int highlightX1;
    int highlightX2;
    int highlightY;
    switch(timeCursor)
    {
        case HOUR:
            highlightX1 = timeAndDateXValue;
            highlightX2 = highlightX1 + charWidth * 2;
            highlightY = timeYValue + 2;
            break;
        case MINUTE:
            highlightX1 = timeAndDateXValue + charWidth * 3;
            highlightX2 = highlightX1 + charWidth * 2;
            highlightY = timeYValue + 2;
            break;
        case SECOND:
            highlightX1 = timeAndDateXValue + charWidth * 6;
            highlightX2 = highlightX1 + charWidth * 2;
            highlightY = timeYValue + 2;
            break;
        case MONTH:
            highlightX1 = timeAndDateXValue;
            highlightX2 = highlightX1 + charWidth * 2;
            highlightY = dateYValue + 2;
            break;
        case DAY:
            highlightX1 = timeAndDateXValue + charWidth * 3;
            highlightX2 = highlightX1 + charWidth * 2;
            highlightY = dateYValue + 2;
            break;
        case YEAR:
            highlightX1 = timeAndDateXValue + charWidth * 6;
            highlightX2 = highlightX1 + charWidth * 2;
            highlightY = dateYValue + 2;
            break;
        case TimeHelpers::CANCEL:
            u8g2->setFont(fontButtons);
            highlightX1 = cancelButtonX;
            highlightX2 = highlightX2 + u8g2->getStrWidth("CANCEL");
            highlightY = buttonYValue + 2;
            break;
        case TimeHelpers::DONE:
            u8g2->setFont(fontButtons);
            highlightX1 = getXOfDoneButton(u8g2);
            highlightX2 = highlightX1 + u8g2->getStrWidth("DONE");
            highlightY = buttonYValue + 2;
            break;
        default:
            break;

    }
    u8g2->drawLine(highlightX1, highlightY, highlightX2, highlightY);
}