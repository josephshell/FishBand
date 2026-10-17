#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <array>
#include <initializer_list>

const std::array<char, 17> blankLine = { "" };

class MenuOption {
public:
  std::array<char, 17> text;
  std::function<void()> callback;

  MenuOption() {
    text = blankLine;
  }

  MenuOption(std::array<char,17> p_text, std::function<void()> p_callback) {
    text = p_text;
    callback = p_callback;
  }

};

const uint8_t MAX_MENU_OPTION_COUNT = 8;

class Menu {
private:
  // The index for the first option on the watch
  int8_t cursor = 0;
  // The number of options available
  uint8_t optionCount = 0;
  // The actual array of MenuOption objects
  std::array<MenuOption, MAX_MENU_OPTION_COUNT> options;

public:
  /**
    * WARNING, 8 MAX OPTIONS
    */
  Menu(std::initializer_list<MenuOption> p_options) {
    uint8_t p_optionCount = p_options.size();
    if (p_optionCount > 8) {
      Serial.println("More than max options in Menu initializer, cutting it off");
    }
    for (int i = 0; i < p_optionCount && i < MAX_MENU_OPTION_COUNT; i++) {
      options[i] = p_options.begin()[i];
    }
    optionCount = p_optionCount;
  }

  std::array<char, 17> textAtCursorOffset(int8_t offset) {
    int8_t offsetCursor = cursor + offset;
    if (offset < 0) {
      offsetCursor = offset + cursor + optionCount;
      if (offsetCursor < 0) {
        Serial.println("Error calculating cursor offset, negative offset greater than option count.");
        offsetCursor = -1;
      }
    }
    if (offsetCursor == -1) {
      return blankLine;
    }
    return options[offsetCursor % optionCount].text;
  }

  void select() {
    if (cursor > optionCount) {
      Serial.println("Error selecting option, cursor is greater than option count.");
      Serial.println("Resetting cursor");
      clear();
      return;
    }
    options[cursor].callback();
  }

  void scrollUp() {
    cursor--;
    if (cursor < 0) {
      cursor = optionCount - 1;
    }
  }

  void scrollDown() {
    cursor++;
    if (cursor >= optionCount) { // if scrolling beyond bottom, make sure it restarts at 0 so as to not miss initial option
      cursor = 0;
    }
  }

  /**
    * Resets the cursor back to start
    */
  void clear() {
    cursor = 0;
  }
};

#endif