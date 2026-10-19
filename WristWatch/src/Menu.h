#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <vector>
#include <cstring>
#include <initializer_list>

const char* blankLine = "";

struct MenuOption {
	char* text = nullptr;
	std::function<void()> callback;

	MenuOption(const char *p_text, std::function<void()> p_callback) {
		callback = p_callback;
		if (p_text) {
			text = new char[std::strlen(p_text) + 1];
			std::strncpy(text, p_text, strlen(p_text) + 1);
		}
	}

	MenuOption(const MenuOption& other) {
		callback = other.callback;
		if (other.text) {
			text = new char[std::strlen(other.text) + 1];
			std::strncpy(text, other.text, strlen(other.text) + 1);
		}
	}

	~MenuOption() {
		delete[] text;
	}
};

const uint8_t MAX_MENU_OPTION_COUNT = 8;

class Menu {
private:
  // The index for the first option on the watch
  int8_t cursor = 0;
  // The actual list of MenuOption objects
  std::vector<MenuOption> options;
public:
	Menu(std::initializer_list<MenuOption> initialOptions) : options(initialOptions) { }

  const char* textAtCursorOffset(int8_t offset) {
	if (options.size() == 0) {
		return blankLine;
	}
    int8_t offsetCursor = cursor + offset;
    if (offset < 0) {
      offsetCursor = offset + cursor + options.size();
      if (offsetCursor < 0) {
        Serial.println("Error calculating cursor offset, negative offset greater than option count.");
        offsetCursor = -1;
      }
    }
    if (offsetCursor == -1) {
      return blankLine;
    }
    return options[offsetCursor % options.size()].text;
  }

  void select() {
	if (options.size() == 0) {
		return;
	}
    if (cursor > options.size()) {
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
		cursor = options.size() - 1;
	}
  }

  void scrollDown() {
	cursor++;
	if (cursor >= options.size()) { // if scrolling beyond bottom, make sure it restarts at 0 so as to not miss initial option
	cursor = 0;
	}
  }

  uint8_t getOptionCount() {
	  return options.size();
  }

  void addOption(const char *text, std::function<void()> callback) {
	options.emplace_back(text, callback);
  }

  /**
    * Resets the cursor back to start
    */
  void clear() {
    cursor = 0;
  }
};

#endif