#include <Arduino.h>
#include <State.h>
#include <time_provider/TimeProvider.h>
#include <time_provider/ds3231/DS3231TimeProvider.h>
#include <ButtonManagement.h>
#include <Esp32C3ButtonImpl.h>
#include <Menu.h>
#include <Display.h>
#include <OledDisplay.h>
#include <alarms/AlarmBuilder.h>
#include <alarms/AlarmManager.h>
#include <BatteryManagement.h>
#include <set_time/TimeSetter.h>
#include <sleep/IdleTimer.h>
#include <update_agents/UpdateAgents.h>
#include <vibration_patterns/VibrationPattern.h>
#include <vibration_handler/VibrationHandler.h>

const int MAX_ALARMS = 8;
// Sleep stored data
RTC_DATA_ATTR int alarmCount;
RTC_DATA_ATTR Alarm storedAlarms[8];

// Time until idle device goes into deep sleep in Main display, Main menu, or alarm list
const int DEFAULT_IDLE_TIMEOUT = 20;
// Time until idle device goes into deep sleep in other states
const int LONGER_IDLE_TIMEOUT = 40;

// TODO: Move state management into a new header
void changeToState(State newState);
void displayByState();
void actionsOnTheSecond();
void goToSleep();

ButtonInterface *b;
TimeProvider *tp;
Display *display;

AlarmBuilder *alarmBuilder;
Alarm alarmBuilderAlarm;

AlarmManager *alarmManager;
Alarm *activeAlarm;

IdleTimer *idleTimer;

VibrationPattern *vibration;
VibrationHandler vibrationHandler = VibrationHandler();

TimeSetter timeSetter;

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
	// Cancel any timer wakeups set
	esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
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
	alarmManager = new AlarmManager(tp);
	if (alarmCount > 0)
	{
		for (int i = 0; i < alarmCount; i++)
		{
			alarmManager->registerAlarm(storedAlarms[i]);
			char alarmString[32];
			storedAlarms[i].toString(alarmString, sizeof(alarmString));
			alarmsMenu.addOption(alarmString, []()
								 {
				Serial.print("In alarms menu, selected alarm");
				changeToState(MAIN_DISPLAY); });
		}
	}
	vibration = new SteadyVibrationPattern();
	idleTimer = new IdleTimer(DEFAULT_IDLE_TIMEOUT, tp);
	activityMonitor = []()
	{ idleTimer->noteActivity(); };

	UpdateAgents::addAgent("Agent Seconds", 1000, actionsOnTheSecond);

	// Set initial state
	changeToState(MAIN_DISPLAY);
}

void loop()
{
	unsigned long currentMillis = millis();
	displayByState();
	UpdateAgents::pollAgents(currentMillis);
	pollButtons();
	if (activeAlarm)
	{
		if (state != ALARM_ACTIVE)
			changeToState(ALARM_ACTIVE);
		if (vibration->isActive(currentMillis))
		{
			vibrationHandler.startVibration();
		}
	}
}

void actionsOnTheSecond()
{
	// Read time
	currentTime = tp->getCurrentTime();

	// Read battery level
	batteryLevel = BatteryManagement::getBatteryLevel();

	// Check alarms to see if they should be going off
	std::vector<Alarm> toTrigger = alarmManager->alarmsMustTrigger();
	if (!toTrigger.empty())
	{
		activeAlarm = new Alarm();
		*activeAlarm = toTrigger.front();
		Serial.print("Alarm triggered: ");
		char alarmString[32];
		activeAlarm->toString(alarmString, sizeof(alarmString));
		Serial.println(alarmString);
		alarmManager->recalculateTriggers();
	}
	if (activeAlarm)
		idleTimer->noteActivity();
	if (idleTimer->isUserIdle())
		goToSleep();
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
		display->settingTime(timeSetter.getAsTm(), timeSetter.getTimeCursor());
		break;
	case ALARM_ACTIVE:
		display->activeAlarm(currentTime);
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
	idleTimer->noteActivity();
	idleTimer->setIdleTimeout(DEFAULT_IDLE_TIMEOUT);
	vibrationHandler.stopVibration();
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
		idleTimer->setIdleTimeout(LONGER_IDLE_TIMEOUT);
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
							Alarm newAlarm{};
							alarmBuilder->buildAlarm(newAlarm);
							time_t nextTriggerTime = alarmManager->registerAlarm(newAlarm);
							char alarmString[32];
							alarmBuilder->alarmToString(alarmString, sizeof(alarmString));
							Serial.println("Finished building alarm!");
							Serial.print("Your new alarm is: ");
							Serial.println(alarmString);
							Serial.print("It will trigger at timestamp: ");
							Serial.println(nextTriggerTime);
							if (alarmCount < MAX_ALARMS)
							{
								storedAlarms[alarmCount] = newAlarm;
								alarmCount++;
								alarmsMenu.addOption(alarmString, []()
								{
									// TODO: Make the function have access to the menu option name
									Serial.print("In alarms menu, selected alarm");
									changeToState(MAIN_DISPLAY);
								});
							} else
							{
								Serial.print("Failed to set new alarm, max alarms reached");
							}
							Serial.println("Returning to main menu");
							changeToState(MAIN_MENU);
						} });
		setCallback(b->sb(), PRESSED, []()
					{ Serial.println("In make alarms, SB was pressed"); alarmBuilder->modifySelected(-1); });
		break;
	case SET_TIME:
		idleTimer->setIdleTimeout(LONGER_IDLE_TIMEOUT);
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
						} });
		setCallback(b->sb(), PRESSED, []()
					{ Serial.println("SET_TIME SB."); timeSetter.decrementTimeSetting(); });
		break;
	case ALARM_ACTIVE:
		idleTimer->setIdleTimeout(LONGER_IDLE_TIMEOUT);
		setCallback(b->bl(), PRESSED, []()
					{ 
						delete activeAlarm;
						activeAlarm = nullptr;
						changeToState(MAIN_DISPLAY); });
		setCallback(b->br(), PRESSED, []()
					{ 
						delete activeAlarm;
						activeAlarm = nullptr;
						changeToState(MAIN_DISPLAY); });
		setCallback(b->st(), PRESSED, []()
					{ 
						delete activeAlarm;
						activeAlarm = nullptr;
						changeToState(MAIN_DISPLAY); });
		setCallback(b->sb(), PRESSED, []()
					{ 
						delete activeAlarm;
						activeAlarm = nullptr;
						changeToState(MAIN_DISPLAY); });
		break;
	default:
		idleTimer->setIdleTimeout(LONGER_IDLE_TIMEOUT);
		Serial.print("Unsupported state? ");
		Serial.println(newState);
	}
}

void goToSleep()
{
	Serial.println("Device going to sleep");
	display->clear(); // Empty display
	time_t nextTrigger = alarmManager->nextTriggerTimestamp();
	if (alarmCount > 0 && nextTrigger != 0)
	{
		time_t sleepStartTime = tp->getCurrentTime();
		time_t secondsUntilAlarm = nextTrigger - sleepStartTime; 
		if (secondsUntilAlarm < 20) // If less than 20 seconds to alarm
		{
			Serial.println("Not sleeping, too close to alarm");
			idleTimer->noteActivity();
			return;
		}
		Serial.print("Will wake before the alarm that triggers at: ");
		Serial.print(nextTrigger);
		Serial.print(". Device will wake in: ");
		Serial.print(secondsUntilAlarm - 3);
		Serial.println(" seconds.");
		// Sleep until 2 seconds before alarm
		uint64_t usToSleep = (secondsUntilAlarm - 3) * 1000000;
		esp_sleep_enable_timer_wakeup(usToSleep);
	}
	esp_deep_sleep_enable_gpio_wakeup(BIT(D1) + BIT(D2), ESP_GPIO_WAKEUP_GPIO_LOW);
	
	// Flush and close serial due to issue with RTC_DATA_ATT saving when connected to serial
	Serial.flush();
	Serial.end();

	esp_deep_sleep_start();
}
