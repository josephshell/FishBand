#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <array>
#include <algorithm>
#include <initializer_list>

const std::array<char, 17> blankLine = { "" };

class MenuOption {
public:
  std::array<char, 17> text;

  MenuOption() {
    text = blankLine;
  }

  MenuOption(std::array<char,17> p_text) {
    text = p_text;
  }

};

class Menu {
public:
  std::array<MenuOption, 8> options;

  /**
    * WARNING, 8 MAX OPTIONS
    */
  Menu(std::initializer_list<MenuOption> p_options) {
    for (int i = 0; i < p_options.size(); i ++) {
      options[i] = p_options.begin()[i];
    }
  }
  // The index for the first option on the watch
  uint8_t cursor = 0;

  // The number of options in the menu (used in scrolling)
  uint8_t optionCount() {
    return std::count_if(options.begin(), options.end(), [](MenuOption opt){
      return opt.text != blankLine;
    });
  }


  std::array<char, 17> option1() {
    if (cursor >= optionCount()) {
      cursor = 0;
      Serial.println("Error in option count math, resetting to start of main menu");
    }
    return options[cursor].text;
  }

  std::array<char, 17> option2() {
    if (cursor + 1 >= optionCount()) {
      return blankLine;
    } else {
      return options[cursor + 1].text;
    }
  }

  void scrollForward() {
    cursor = cursor + 2;
    if (cursor >= optionCount()) { // if scrolling beyond bottom, make sure it restarts at 0 so as to not miss initial option
      cursor = 0;
    }
  }

  void clear() {
    cursor = 0;
  }
};

#endif