#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Arduino.h>
#include <Display.h>
#include <Menu.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <WatchFaces.h>
#include <string>

class OledDisplay : public Display {
private:
  U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2;
public:
  OledDisplay() : u8g2(U8G2_R0, U8X8_PIN_NONE) {}

  void initialize(ButtonInterface* b) override {
    u8g2.begin(b->br(), b->sb(), b->st(), U8X8_PIN_NONE, U8X8_PIN_NONE, b->bl());
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
      u8g2.setFont(u8g2_font_spleen8x16_mf);
      u8g2.drawStr(0, 15, dayBuffer);
      u8g2.setFont(u8g2_font_6x10_mn);
      u8g2.drawStr(0, 25, dateBuffer);
      u8g2.setFont(u8g2_font_inr16_mn);
      u8g2.drawStr(32, 42, timeBuffer);
      u8g2.drawXBMP(0, 0, 128, 64, epd_bitmap_WatchFace_cornergecko);
    } while (u8g2.nextPage());
  }

  void displayMenu(Menu& menu) override {
    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_spleen8x16_mf);
      u8g2.drawStr(0, 15, menu.option1Text().data());
      u8g2.drawStr(0, 35, menu.option2Text().data());
    } while (u8g2.nextPage());
  }

  void displayImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap) override {
    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_spleen8x16_mf);
      u8g2.drawXBMP(x, y, w, h, bitmap);
    } while (u8g2.nextPage());
  }

  MenuOption* openMenu(MenuOption options[], uint8_t optionCount) override {
    std::string optionsString = "";
    for (int i = 0; i < optionCount; i++) {
      optionsString.append(options[i].text.data());
      if (i < optionCount - 1) {
        optionsString.append("\n");
      }
    }
    uint8_t selection = 0;
    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_spleen8x16_mf);
      selection = u8g2.userInterfaceSelectionList("Options", 2, optionsString.data());
    } while (u8g2.nextPage());
    if (selection >= 0 && selection < optionCount) {
      return &options[selection - 1]; // result from u8g2 is 1-indexed
    }
  }

  void clear() override {
    u8g2.clear();
  }

};
#endif