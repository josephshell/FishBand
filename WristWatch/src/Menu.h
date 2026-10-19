#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <vector>
#include <cstring>
#include <initializer_list>

const char *blankLine = "";

/**
 * Represents a menu option. Has a callback and name.
 * Name char array is managed by this struct
 */
struct MenuOption
{
	char *text = nullptr;
	std::function<void()> callback;

	/**
	 * Constructor for MenuOption. Takes a text and a callback function.
	 * @param p_text The text for the menu option.
	 * @param p_callback The callback function to be called when the option is selected.
	 */
	MenuOption(const char *p_text, std::function<void()> p_callback)
	{
		callback = p_callback;
		if (p_text)
		{
			text = new char[std::strlen(p_text) + 1];
			std::strncpy(text, p_text, strlen(p_text) + 1);
		}
	}

	/**
	 * Copy constructor for MenuOption. Copies the text and callback from another MenuOption.
	 * @param other The MenuOption to copy from.
	 */
	MenuOption(const MenuOption &other)
	{
		callback = other.callback;
		if (other.text)
		{
			text = new char[std::strlen(other.text) + 1];
			std::strncpy(text, other.text, strlen(other.text) + 1);
		}
	}

	~MenuOption()
	{
		delete[] text;
	}
};

/**
 * Represents a menu with options and a cursor. Allows scrolling through options and selecting them.
 */
class Menu
{
private:
	int8_t cursor = 0;
	std::vector<MenuOption> options;

public:
	Menu(std::initializer_list<MenuOption> initialOptions) : options(initialOptions) {}

	/**
	 * Returns the text for a menu option at the cursor, offset by the offset value.
	 */
	const char *textAtCursorOffset(int8_t offset)
	{
		if (options.size() == 0)
		{
			return blankLine;
		}
		int8_t offsetCursor = cursor + offset;
		if (offset < 0)
		{
			offsetCursor = offset + cursor + options.size();
			if (offsetCursor < 0)
			{
				Serial.println("Error calculating cursor offset, negative offset greater than option count.");
				offsetCursor = -1;
			}
		}
		if (offsetCursor == -1)
		{
			return blankLine;
		}
		return options[offsetCursor % options.size()].text;
	}

	/**
	 * Activates the callback for the menu option at cursor.
	 */
	void select()
	{
		if (options.size() == 0)
		{
			return;
		}
		if (cursor > options.size())
		{
			Serial.println("Error selecting option, cursor is greater than option count.");
			Serial.println("Resetting cursor");
			clear();
			return;
		}
		options[cursor].callback();
	}

	void scrollUp()
	{
		cursor--;
		if (cursor < 0)
		{
			cursor = options.size() - 1;
		}
	}

	void scrollDown()
	{
		cursor++;
		if (cursor >= options.size())
		{ // if scrolling beyond bottom, make sure it restarts at 0 so as to not miss initial option
			cursor = 0;
		}
	}

	uint8_t getOptionCount()
	{
		return options.size();
	}

	void addOption(const char *text, std::function<void()> callback)
	{
		options.emplace_back(text, callback);
	}

	/**
	 * Resets the cursor back to start
	 */
	void clear()
	{
		cursor = 0;
	}
};

#endif