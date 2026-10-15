#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Arduino.h>
#include <Display.h>
#include <Menu.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <WatchFaces.h>

const uint8_t *font_16_px = u8g2_font_inr16_mn;
const uint8_t *font_10_px = u8g2_font_spleen8x16_mf;
const uint8_t *font_8_px = u8g2_font_6x10_mn;

class OledDisplay : public Display {
private:
  U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2;
public:
  OledDisplay() : u8g2(U8G2_R0, U8X8_PIN_NONE) {}

  void initialize() override {
    u8g2.begin();
  }

  void displayDateTime(std::time_t& time) override {
    std::tm* localTime = std::localtime(&time);
    char dayBuffer[4];
    char dateBuffer[17];
    char timeBuffer[17];
    std::strftime(dayBuffer, sizeof(dayBuffer), "%a", localTime);
    std::strftime(dateBuffer, sizeof(dateBuffer), "%m/%d", localTime);
    std::strftime(timeBuffer, sizeof(timeBuffer), "%I:%M %p", localTime);
    u8g2.firstPage();
    do {
      u8g2.setBitmapMode(1);
      u8g2.setFont(font_10_px);
      u8g2.drawStr(0, 15, dayBuffer);
      u8g2.setFont(font_8_px);
      u8g2.drawStr(0, 25, dateBuffer);
      u8g2.setFont(font_16_px);
      u8g2.drawStr(32, 42, timeBuffer);
      u8g2.drawXBMP(0, 0, 128, 64, epd_bitmap_WatchFace_cornergecko);
    } while (u8g2.nextPage());
  }

  void displayMenu(Menu& menu) override {
    u8g2.firstPage();
    do {
      u8g2.setFont(font_10_px);
      u8g2.drawStr(0, 15, menu.option1Text().data());
      u8g2.drawStr(0, 35, menu.option2Text().data());
    } while (u8g2.nextPage());
  }

  void displayImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap) override {
    u8g2.firstPage();
    do {
      u8g2.drawXBMP(x, y, w, h, bitmap);
    } while (u8g2.nextPage());
  }

  void clear() override {
    u8g2.clear();
  }

};
#endif