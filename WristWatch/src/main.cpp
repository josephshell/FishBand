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
#include <alarms/TriggerableAlarm.h>
#include <BatteryManagement.h>
#include <update_agents/UpdateAgents.h>
#include <set_time/TimeSetter.h>

// TODO: Move state management into a new header
void changeToState(State newState);
void displayByState();
void actionsOnTheSecond();

void handleAlarmTriggered(TriggerableAlarm &alarm);

ButtonInterface *b;
TimeProvider *tp;
Display *display;

AlarmBuilder *alarmBuilder;
Alarm alarmBuilderAlarm;

TriggerableAlarm *activeAlarm = nullptr;
std::vector<TriggerableAlarm> alarms;

TimeSetter timeSetter;
tm *timeSetterLocalTimePtr = new tm();

time_t currentTime = 0;

Menu mainMenu(
	{MenuOption("Set Time", []()
				{ changeToState(SET_TIME); }),
	 MenuOption("ls Alarms", []()
				{ changeToState(SHOW_ALARMS); }),
	 MenuOption("Make Alarm", []()
				{ changeToState(MAKE_ALARM); }),
	 MenuOption("Test Page", []()
				{ changeToState(TEST_PAGE); }),
	 MenuOption("Return", []()
				{ changeToState(MAIN_DISPLAY); })});
Menu alarmsMenu({MenuOption("Return", []()
							{ changeToState(MAIN_MENU); })});

int8_t batteryLevel = 100;
unsigned long batteryUpdateDelay = 30000;

State state = MAIN_DISPLAY;

void setup()
{
	// Configure Serial for logging
	Serial.begin(9600);

	BatteryManagement::initialize();

	// region: Configuration
	b = new Esp32C3ButtonImpl();
	tp = new DS3231TimeProvider();
	display = new OledDisplay();
	// endregion

	configureButtons(*b);
	display->initialize();
	tp->initialize();

	UpdateAgents::addAgent("Agent Seconds", 1000, actionsOnTheSecond);

	// Set initial state
	changeToState(MAIN_DISPLAY);
}

void loop()
{
	unsigned long currentMillis = millis();
	if (activeAlarm)
	{
		changeToState(SOUND_ALARM);
	}
	displayByState();
	UpdateAgents::pollAgents(currentMillis);
	pollButtons();
}

void actionsOnTheSecond()
{
	// Read time
	currentTime = tp->getCurrentTime();

	// Read battery level
	batteryLevel = BatteryManagement::getBatteryLevel();

	// Check alarms to see if they should be going off
	
	if (!activeAlarm) {
		for (TriggerableAlarm &alarm : alarms)
		{
			if (alarm.shouldTrigger(currentTime))
			{
				activeAlarm = &alarm;
				handleAlarmTriggered(alarm);
			}
		}
	}
}

/**
 * Displays the current date and time on the LCD
 */
void showMainDisplay()
{
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
	case SET_TIME:
		TimeHelpers::getLocalTime(timeSetterLocalTimePtr, timeSetter.getCurrentTimeSetting());
		display->settingTime(timeSetterLocalTimePtr, timeSetter.getTimeCursor());
		break;
	case SOUND_ALARM:
		if (activeAlarm)
		{
			Alarm *alarm = activeAlarm->getAlarm();
			display->displayAlarm(*alarm, HOUR);
		}
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
							Alarm newAlarm;
							alarmBuilder->buildAlarm(newAlarm);
							Serial.println(newAlarm.toString(buffer, sizeof(buffer)));
							alarmsMenu.addOption(buffer, []()
							{
								// TODO: Make the function have access to the menu option name
								Serial.print("In alarms menu, selected alarm");
								changeToState(MAIN_DISPLAY);
							});
							TriggerableAlarm newTriggerableAlarm(newAlarm);
							newTriggerableAlarm.recalculateNextTriggerTime(currentTime);
							alarms.emplace_back(newTriggerableAlarm);
							Serial.println("New alarm debug info:");
							Serial.print("Current time: ");
							Serial.print(currentTime);
							Serial.print(" next alarm trigger time: ");
							Serial.println(newTriggerableAlarm.getNextTriggerTime());
							Serial.println("Returning to main menu");
							changeToState(MAIN_MENU);
						} });
		setCallback(b->sb(), PRESSED, []()
					{ Serial.println("In make alarms, SB was pressed"); alarmBuilder->modifySelected(-1); });
		break;
	case SET_TIME:
		timeSetter.initialize(currentTime);
		setCallback(b->bl(), PRESSED, []()
					{ Serial.println("SET_TIME BL."); timeSetter.decrementCursor(); });
		setCallback(b->br(), PRESSED, []()
					{ Serial.println("SET_TIME BR."); timeSetter.incrementCursor(); });
		setCallback(b->st(), PRESSED, []()
					{
						Serial.println("SET_TIME ST.");
						int result = timeSetter.incrementTimeSetting(); 
						if (result == 1) {
							Serial.println("User requested to save the time.");
							tp->setCurrentTime(timeSetter.getCurrentTimeSetting());
							changeToState(MAIN_DISPLAY);
						} else if (result == -1) {
							Serial.println("User requested to cancel setting the time.");
							changeToState(MAIN_DISPLAY);
						}
					});
		setCallback(b->sb(), PRESSED, []()
					{ Serial.println("SET_TIME SB."); timeSetter.decrementTimeSetting(); });
		break;
	case SOUND_ALARM:
		setCallback(b->bl(), PRESSED, []()
					{ Serial.println("SOUND_ALARM BL."); activeAlarm = nullptr; changeToState(MAIN_DISPLAY); });
		setCallback(b->br(), PRESSED, []()
					{ Serial.println("SOUND_ALARM BR."); activeAlarm = nullptr; changeToState(MAIN_DISPLAY); });
		setCallback(b->st(), PRESSED, []()
					{ Serial.println("SOUND_ALARM ST."); activeAlarm = nullptr; changeToState(MAIN_DISPLAY); });
		setCallback(b->sb(), PRESSED, []()
					{ Serial.println("SOUND_ALARM SB."); activeAlarm = nullptr; changeToState(MAIN_DISPLAY); });
		break;
	default:
		Serial.print("Unsupported state? ");
		Serial.println(newState);
	}
}

void handleAlarmTriggered(TriggerableAlarm &alarm)
{
	Serial.print("Alarm triggered: ");
	char buffer[32];
	Serial.println(alarm.toString(buffer, sizeof(buffer)));
}
