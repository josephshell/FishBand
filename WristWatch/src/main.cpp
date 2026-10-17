#include <Arduino.h>
#include <State.h>
#include <TimeProvider.h>
#include <ButtonManagement.h>
#include <Esp32C3ButtonImpl.h>
#include <Menu.h>
#include <Display.h>
#include <OledDisplay.h>
#include <WatchFaces.h>
#include <AlarmBuilder.h>

ButtonInterface *b;
TimeProvider *tp;

// TODO: Move state management into a new header
void changeToState(State newState);
void displayByState();

Display *display;
AlarmBuilder *alarmBuilder;

Menu mainMenu({MenuOption("ls Alarms", []()
						  { changeToState(SHOW_ALARMS); }),
			   MenuOption("Make Alarm", []()
						  { changeToState(MAKE_ALARM); }),
			   MenuOption("Test Page", []()
						  { changeToState(TEST_PAGE); }),
			   MenuOption("Return", []()
						  { changeToState(DISPLAY_TIME); })});
Menu alarmsMenu({MenuOption("01:23 Once", []()
							{ changeToState(DISPLAY_TIME); }),
				 MenuOption("17:55 WkDy", []()
							{ changeToState(DISPLAY_TIME); })});

int lastMoveTime = 0;
State state = DISPLAY_TIME;

void setup()
{
	// Configure Serial for logging
	Serial.begin(9600);

	// region: Configuration
	b = new Esp32C3ButtonImpl();
	tp = new DS1307TimeProvider();
	display = new OledDisplay();
	// endregion

	configureButtons(*b);
	display->initialize();
	tp->initialize();

	// Set initial state to time display
	changeToState(DISPLAY_TIME);
}

void loop()
{
	pollButtons();
	displayByState();
}

/**
 * Displays the current date and time on the LCD
 */
void displayTime()
{
	std::time_t currentTime = tp->getCurrentTime();
	display->displayDateTime(currentTime);
}

/**
 * Runs each cycle, updating the lcd based on the current State
 */
void displayByState()
{
	switch (state)
	{
	case DISPLAY_TIME:
		displayTime();
		break;
	case MAIN_MENU:
		display->displayMenu(mainMenu);
		break;
	case TEST_PAGE:
		display->runTest();
		break;
	case SHOW_ALARMS:
		display->displayMenu(alarmsMenu);
		break;
	case MAKE_ALARM:
		display->displayAlarmBuilder(alarmBuilder);
		break;
	default:
		break;
	}
}

void configureMenuButtons(Menu &menu)
{
	setCallback(b->bl(), PRESSED, []()
				{
    Serial.println("In menu, BL was pressed. Returning to time display");
    changeToState(DISPLAY_TIME); });
	setCallback(b->br(), PRESSED, [&menu]()
				{
    Serial.print("In menu, BR was pressed. Selecting menu option: ");
    Serial.println(menu.textAtCursorOffset(0).data());
    menu.select(); });
	setCallback(b->st(), PRESSED, [&menu]()
				{
    Serial.println("In menu, ST was pressed. Scrolling up");
    menu.scrollUp(); });
	setCallback(b->sb(), PRESSED, [&menu]()
				{
    Serial.println("In menu, SB was pressed. Scrolling down");
    menu.scrollDown(); });
}

/**
 * Called to change the watch to a new State [newState].
 * Removes all button observers and clears the lcd. Button observers
 * Should be registered here.
 */
void changeToState(State newState)
{
	resetButtonCallbacks();
	display->clear();
	state = newState;
	switch (newState)
	{
	case DISPLAY_TIME:
		setCallback(b->bl(), PRESSED, []()
					{
        Serial.println("In display time, BL was pressed. Going to main menu.");
        changeToState(MAIN_MENU); });
		setCallback(b->br(), PRESSED, []()
					{ Serial.println("In display time, BR was pressed"); });
		setCallback(b->st(), PRESSED, []()
					{ Serial.println("In display time, ST was pressed"); });
		setCallback(b->sb(), PRESSED, []()
					{ Serial.println("In display time, SB was pressed"); });
		break;
	case MAIN_MENU:
		mainMenu.clear();
		configureMenuButtons(mainMenu);
		break;
	case SHOW_ALARMS:
		alarmsMenu.clear();
		configureMenuButtons(alarmsMenu);
		break;
	case TEST_PAGE:

		break;
	case MAKE_ALARM:
		if (!alarmBuilder)
		{
			alarmBuilder = new AlarmBuilder();
		}
		break;
	default:
		Serial.print("Unsupported state? ");
		Serial.println(newState);
	}
}
