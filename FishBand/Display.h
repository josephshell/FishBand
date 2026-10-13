#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
// TODO: REMOVE LCD CODE FROM INTERFACE FILE
#include <LiquidCrystal.h>
#include <ctime>

class Display {
public:
  virtual void initialize() = 0;

  virtual void displayDateTime(std::time_t& time) = 0;

  virtual void displayMenu(Menu& menu) = 0;

  virtual void displayImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap) = 0;

  virtual void clear() = 0;

  virtual ~Display() = default;
};

class LcdDisplay : public Display {
private:
  LiquidCrystal lcd = LiquidCrystal(D6, D5, D7, D8, D9, D10);
public:
  void initialize() override {
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
  }
  /**
  * Displays the current date and time on the LCD
  */
  void displayDateTime(std::time_t &time) override {
    // 2. Convert to the local time structure
    std::tm* localTime = std::localtime(&time);
    lcd.home();
    printTimeToLcd(localTime);
    lcd.setCursor(0, 1);
    printDateToLcd(localTime);
  }

  void printDateToLcd(std::tm* localTime) {
    char buffer[17];
    std::strftime(buffer, sizeof(buffer), "%a, %m/%d/%y", localTime);
    lcd.print(buffer);
  }

  void printTimeToLcd(std::tm* localTime) {
    // int timeAlive = millis() / 1000;
    // char timeBuffer[9]; // Enough space for "HH:MM:SS/0"
    // int hours = timeAlive / (60 * 60);
    // int minutes = (timeAlive / 60) % 60;
    // int seconds = timeAlive % 60;
    // // Format the string safely into the buffer
    // snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", hours, minutes, seconds);
    // lcd.print(timeBuffer);
    char buffer[17];
    std::strftime(buffer, sizeof(buffer), "%I:%M:%S %p", localTime);
    lcd.print(buffer);
  }

  /**
  * Displays the main menu on the LCD
  */
  void displayMenu(Menu& menu) override {
    lcd.home();
    lcd.print(menu.option1Text().data());
    lcd.setCursor(0, 1);
    lcd.print(menu.option2Text().data());
  }

  void displayImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap) override {
    if (Serial) {
      Serial.println("LCD does not support images");
    }
  }


  void clear() override {
    lcd.clear();
  }
};

#endif