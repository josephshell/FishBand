#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Arduino.h>
#include <Display.h>
#include <Menu.h>
#include <U8g2lib.h>
#include <Wire.h>

class OledDisplay : public Display {
private:
  U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2;
  time_t lastTimeDisplayed;
public:
  OledDisplay() : u8g2(U8G2_R0, U8X8_PIN_NONE) {}

  void initialize() override {
    u8g2.begin();
  }

  void displayDateTime(std::time_t& time) override {
    if (time == lastTimeDisplayed) {
      return;
    }
    std::tm* localTime = std::localtime(&time);
    char dateBuffer[17];
    char timeBuffer[17];
    std::strftime(dateBuffer, sizeof(dateBuffer), "%a, %m/%d/%y", localTime);
    std::strftime(timeBuffer, sizeof(timeBuffer), "%I:%M:%S %p", localTime);
    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_ncenR14_tr);
      u8g2.drawStr(0, 15, timeBuffer);
      u8g2.drawStr(0, 35, dateBuffer);
    } while (u8g2.nextPage());
  }

  void displayMenu(Menu& menu) override {
    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_ncenR14_tr);
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