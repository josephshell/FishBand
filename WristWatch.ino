// include the library code:
#include <LiquidCrystal.h>
#include <State.h>
#include <ButtonManagement.h>
#include <MainMenu.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(D6, D5, D7, D8, D9, D10);
MainMenu mainMenu;

int lastMoveTime = 0;
State state = DISPLAY_TIME;

void setup() {
  // Configure Serial for logging
  Serial.begin(9600);
  // Set button pins to use pullup resistor
  pinMode(BL, INPUT_PULLUP);
  pinMode(BR, INPUT_PULLUP);
  pinMode(ST, INPUT_PULLUP);
  pinMode(SB, INPUT_PULLUP);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Set initial state to time display
  changeToState(DISPLAY_TIME);
}

void loop() {
  pollButtons();
  display();
}

/**
 * Runs each cycle, updating the lcd based on the current State
*/
void display() {
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
  lcd.home();
  printDateToLcd();
  lcd.setCursor(0, 1);
  printTimeToLcd();
}

void printDateToLcd() {
  lcd.print("Tue, 09-22-26");
}

void printTimeToLcd() {
  int timeAlive = millis() / 1000;
  char timeBuffer[9]; // Enough space for "HH:MM:SS/0"
  int hours = timeAlive / (60 * 60);
  int minutes = (timeAlive / 60) % 60;
  int seconds = timeAlive % 60;
  // Format the string safely into the buffer
  snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", hours, minutes, seconds);
  lcd.print(timeBuffer);
}

/**
 * Displays the main menu on the LCD
*/
void displayMenu() {
  lcd.home();
  lcd.print(mainMenu.option1().data());
  lcd.setCursor(0, 1);
  lcd.print(mainMenu.option2().data());
}

/**
 * Called to change the watch to a new State [newState].
 * Removes all button observers and clears the lcd. Button observers
 * Should be registered here.
*/
void changeToState(State newState) {
  resetButtonCallbacks();
  lcd.clear();
  state = newState;
  switch (newState) {
    case DISPLAY_TIME:
      setCallback(BL, PRESSED, []() {
        Serial.println("In display time, BL was pressed. Going to main menu.");
        changeToState(MAIN_MENU);
      });
      setCallback(BR, PRESSED, []() {
        Serial.println("In display time, BR was pressed");
      });
      setCallback(ST, PRESSED, []() {
        Serial.println("In display time, ST was pressed");
      });
      setCallback(SB, PRESSED, []() {
        Serial.println("In display time, SB was pressed");
      });
      break;
    case MAIN_MENU:
      mainMenu.clear();
      setCallback(BL, PRESSED, []() {
        Serial.println("In menu, BL was pressed. Returning to time display");
        changeToState(DISPLAY_TIME);
      });
      setCallback(BR, PRESSED, []() {
        Serial.println("In menu, BR was pressed. Scrolling down");
        lcd.clear();
        mainMenu.scrollForward();
      });
      setCallback(ST, PRESSED, []() {
        Serial.print("In menu, ST was pressed. Option 1 Selected: ");
        Serial.println(mainMenu.option1().data());
      });
      setCallback(SB, PRESSED, []() {
        Serial.print("In menu, SB was pressed. Option 2 Selected: ");
        Serial.println(mainMenu.option2().data());
      });
      break;
    default:
      Serial.print("Unsupported state? ");
      Serial.println(newState);
  }
}