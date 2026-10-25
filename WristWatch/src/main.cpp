#include <Arduino.h>
#include <State.h>
#include <TimeProvider.h>
#include <ButtonManagement.h>
#include <Esp32C3ButtonImpl.h>
#include <Menu.h>
#include <Display.h>
#include <OledDisplay.h>
#include <WatchFaces.h>
#include <alarms/AlarmBuilder.h>
#include <BatteryManagement.h>

ButtonInterface *b;
TimeProvider *tp;

// TODO: Move state management into a new header
void changeToState(State newState);
void displayByState();

Display *display;
AlarmBuilder *alarmBuilder;

Menu mainMenu(
	{MenuOption("ls Alarms", []()
				{ changeToState(SHOW_ALARMS); }),
	 MenuOption("Make Alarm", []()
				{ changeToState(MAKE_ALARM); }),
	 MenuOption("Test Page", []()
				{ changeToState(TEST_PAGE); }),
	 MenuOption("Return", []()
				{ changeToState(MAIN_DISPLAY); })});
Menu alarmsMenu({MenuOption("Return", []()
							{ changeToState(MAIN_MENU); })});

Alarm alarmBuilderAlarm;

int lastMoveTime = 0;
State state = MAIN_DISPLAY;

int8_t batteryLevel = 100;
unsigned long lastBattCheck = 0;
unsigned long batteryUpdateFrequency = 1000;

void setup()
{
	// Configure Serial for logging
	Serial.begin(9600);

	BatteryManagement::initialize();

	// region: Configuration
	b = new Esp32C3ButtonImpl();
	tp = new DS1307TimeProvider();
	display = new OledDisplay();
	// endregion

	configureButtons(*b);
	display->initialize();
	tp->initialize();

	// Set initial state
	changeToState(MAIN_DISPLAY);
}

void loop()
{
	unsigned long currentMillis = millis();
	pollButtons();
	displayByState();
	if (currentMillis - lastBattCheck >= batteryUpdateFrequency)
	{
		batteryLevel = BatteryManagement::getBatteryLevel();
		lastBattCheck = currentMillis;
		Serial.print("Battery level updated. New level: ");
		Serial.println(batteryLevel);
	}
}

/**
 * Displays the current date and time on the LCD
 */
void showMainDisplay()
{
	std::time_t currentTime = tp->getCurrentTime();
	display->mainDisplay(currentTime, batteryLevel);
}

/**
 * Runs each cycle, updating the lcd based on the current State
 */
void displayByState()
{
	switch (state)
	{
	case MAIN_DISPLAY:
		showMainDisplay();
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
		alarmBuilder->buildAlarm(alarmBuilderAlarm);
		display->displayAlarm(alarmBuilderAlarm, alarmBuilder->getCursorLocation());
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
    changeToState(MAIN_DISPLAY); });
	setCallback(b->br(), PRESSED, [&menu]()
				{
    Serial.print("In menu, BR was pressed. Selecting menu option: ");
    Serial.println(menu.textAtCursorOffset(0));
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
	if (alarmBuilder)
	{
		alarmBuilderAlarm.clear();
		delete alarmBuilder;
		alarmBuilder = nullptr;
	}
	state = newState;
	switch (newState)
	{
	case MAIN_DISPLAY:
		setCallback(b->bl(), PRESSED, []()
					{
        Serial.println("In main display, BL was pressed. Going to main menu.");
        changeToState(MAIN_MENU); });
		setCallback(b->br(), PRESSED, []()
					{ Serial.println("In main display, BR was pressed"); });
		setCallback(b->st(), PRESSED, []()
					{ Serial.println("In main display, ST was pressed"); });
		setCallback(b->sb(), PRESSED, []()
					{ Serial.println("In main display, SB was pressed"); });
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
		setCallback(b->bl(), PRESSED, []()
					{ Serial.println("In make alarms, BL was pressed"); alarmBuilder->cursorBackward(); });
		setCallback(b->br(), PRESSED, []()
					{ Serial.println("In make alarms, BR was pressed"); alarmBuilder->cursorForward(); });
		setCallback(b->st(), PRESSED, []()
					{
						Serial.println("In make alarms, ST was pressed");
						if (alarmBuilder->modifySelected(1) == 0) {

							Serial.println("Finished building alarm!");
							Serial.print("Your new alarm is: ");
							char buffer[32];
							Serial.println(alarmBuilder->alarmToString(buffer, sizeof(buffer)));
							alarmsMenu.addOption(buffer, []()
							{
								// TODO: Make the function have access to the menu option name
								Serial.print("In alarms menu, selected alarm");
								changeToState(MAIN_DISPLAY);
							});
							Serial.println("Returning to main menu");
							changeToState(MAIN_MENU);
						} });
		setCallback(b->sb(), PRESSED, []()
					{ Serial.println("In make alarms, SB was pressed"); alarmBuilder->modifySelected(-1); });
		break;
	default:
		Serial.print("Unsupported state? ");
		Serial.println(newState);
	}
}
