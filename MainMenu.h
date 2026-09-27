#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <Arduino.h>
#include <array>


class MenuOption {
public:
  std::array<char, 17> text;

  MenuOption(std::array<char,17> p_text) {
    text = p_text;
  }
};

const MenuOption options[3] = {
  MenuOption({ "SHOW TIME" }),
  MenuOption({ "ALARM | TIMER" }),
  MenuOption({ "GO BACK" })
};
const MenuOption blankLine({ "" });

uint8_t optionCount = sizeof(options) / sizeof(options[0]);

class MainMenu {
public:
  MainMenu() {}
  // The index for the first option on the watch
  uint8_t cursor = 0;

  std::array<char, 17> option1() {
    if (cursor >= optionCount) {
      cursor = 0;
      Serial.println("Error in option count math, resetting to start of main menu");
    }
    return options[cursor].text;
  }

  std::array<char, 17> option2() {
    if (cursor + 1 >= optionCount) {
      return blankLine.text;
    } else {
      return options[cursor + 1].text;
    }
  }

  void scrollForward() {
    cursor = cursor + 2;
    if (cursor >= optionCount) { // if scrolling beyond bottom, make sure it restarts at 0 so as to not miss initial option
      cursor = 0;
    }
  }

  void clear() {
    cursor = 0;
  }
};

#endif