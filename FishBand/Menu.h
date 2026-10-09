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
  uint8_t cursor = 0;
  // The number of options available
  uint8_t optionCount = 0;
  // The actual array of MenuOption objects
  std::array<MenuOption, MAX_MENU_OPTION_COUNT> options;

  MenuOption option1() {
    return options[cursor];
  }

  MenuOption option2() {
    return options[cursor + 1];
  }

  /**
  * Logs some data about the menu to the Serial port
  */
  void logDebug() {
    Serial.print("Cursor at ");
    Serial.println(cursor);
    Serial.print("Option count is: ");
    Serial.println(optionCount);
    Serial.print("Has option1: ");
    Serial.println(hasOption1());
    Serial.print("Has option2: ");
    Serial.println(hasOption2());
  }

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
  /**
    * Returns true if there is an option at cursor
    */
  bool hasOption1() {
    return optionCount > 0 && cursor < optionCount;
  }

  /**
    * Returns true if there is an option after the cursor
    */
  bool hasOption2() {
    return optionCount > 1 && (cursor + 1) < optionCount;
  }


  /**
    * Returns a char array of the text from option 1, or blank if there is no option at the cursor
    */
  std::array<char, 17> option1Text() {
    if (hasOption1()) {
      return option1().text;
    } else {
      Serial.println("Trying to access option1 text when no option 1 available.");
      logDebug();
      return blankLine;
    }
  }

  /**
    * Returns a char array of the text from option 2, or blank if there is nothing in option 2
    */
  std::array<char, 17> option2Text() {
    if (hasOption2()) {
      return option2().text;
    } else {
      return blankLine;
    }
  }
  
  /**
    * Calls the callback associated with the MenuOption at option 1 (cursor)
    */
  void option1Press() {
    if (hasOption1()) {
      MenuOption option = option1();
      option.callback();
    } else {
      Serial.println("Error selecting option 1, option 1 not found.");
      logDebug();
    }
  }

  /**
    * Calls the callback associated with the MenuOption at option 2
    */
  void option2Press() {
    if (hasOption2()) {
      MenuOption option = option2();
      option.callback();
    } else {
      Serial.print("Error selecting option 2, option 2 not found");
      logDebug();
    }
  }

  /**
    * Moves the cursor down one page (2 options). Wraps cursor back around if it goes beyond the end of the options.
    */
  void scrollForward() {
    cursor = cursor + 2;
    if (cursor >= optionCount) { // if scrolling beyond bottom, make sure it restarts at 0 so as to not miss initial option
      cursor = 0;
    }
    Serial.println("Post scroll debug");
    logDebug();
  }

  /**
    * Resets the cursor back to start
    */
  void clear() {
    cursor = 0;
  }
};

#endif