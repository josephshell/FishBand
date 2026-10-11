// include the library code:
#include <LiquidCrystal.h>
#include <State.h>
#include <ButtonManagement.h>
#include <Esp32C3ButtonImpl.h>
#include <Menu.h>
#include <Display.h>

ButtonInterface* b;

//TODO: Move state management into a new header
void changeToState(State newState);

Display* display;

Menu mainMenu({
  MenuOption({ "SHOW TIME" }, []() { changeToState(DISPLAY_TIME); }),
  MenuOption({ "ALARM | TIMER" }, []() { changeToState(DISPLAY_TIME); }),
  MenuOption({ "GO BACK" }, []() { changeToState(DISPLAY_TIME); })
});

int lastMoveTime = 0;
State state = DISPLAY_TIME;

void setup() {
  // Configure Serial for logging
  Serial.begin(9600);

  // region: Configuration
  b = new Esp32C3ButtonImpl();
  display = new LcdDisplay();
  // endregion

  configureButtons(*b);
  display->initialize();

  // Set initial state to time display
  changeToState(DISPLAY_TIME);
}

void loop() {
  pollButtons();
  displayByState();
}

/**
 * Runs each cycle, updating the lcd based on the current State
*/
void displayByState() {
  switch(state) {
    case DISPLAY_TIME:
      displayTime();
      break;
    case MAIN_MENU:
      displayMenu();
      break;
    default:
      break;
  }
}

/**
 * Displays the current date and time on the LCD
*/
void displayTime() {
  std::time_t currentTime = std::time(nullptr);
  display->displayDateTime(currentTime);
}

/**
 * Displays the main menu on the LCD
*/
void displayMenu() {
  display->displayMenu(mainMenu);
}


/**
 * Called to change the watch to a new State [newState].
 * Removes all button observers and clears the lcd. Button observers
 * Should be registered here.
*/
void changeToState(State newState) {
  resetButtonCallbacks();
  display->clear();
  state = newState;
  switch (newState) {
    case DISPLAY_TIME:
      setCallback(b->bl(), PRESSED, []() {
        Serial.println("In display time, BL was pressed. Going to main menu.");
        changeToState(MAIN_MENU);
      });
      setCallback(b->br(), PRESSED, []() {
        Serial.println("In display time, BR was pressed");
      });
      setCallback(b->st(), PRESSED, []() {
        Serial.println("In display time, ST was pressed");
      });
      setCallback(b->sb(), PRESSED, []() {
        Serial.println("In display time, SB was pressed");
      });
      break;
    case MAIN_MENU:
      mainMenu.clear();
      setCallback(b->bl(), PRESSED, []() {
        Serial.println("In menu, BL was pressed. Returning to time display");
        changeToState(DISPLAY_TIME);
      });
      setCallback(b->br(), PRESSED, []() {
        Serial.println("In menu, BR was pressed. Scrolling down");
        display->clear();
        mainMenu.scrollForward();
      });
      setCallback(b->st(), PRESSED, []() {
        Serial.print("In menu, ST was pressed. Option 1 Selected: ");
        Serial.println(mainMenu.option1Text().data());
        mainMenu.option1Press();
      });
      setCallback(b->sb(), PRESSED, []() {
        Serial.print("In menu, SB was pressed. Option 2 Selected: ");
        Serial.println(mainMenu.option2Text().data());
        mainMenu.option2Press();
      });
      break;
    case ALARMS:

      break;
    default:
      Serial.print("Unsupported state? ");
      Serial.println(newState);
  }
}