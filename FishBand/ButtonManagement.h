#ifndef BUTTON_MANAGEMENT_H
#define BUTTON_MANAGEMENT_H

#include <Arduino.h>
#include <ButtonInterface.h>

/**
 * Millisecond time to wait between reading button states to insure
 * that bounces in the button press do not register as multiple presses.
*/
const uint8_t DEBOUNCE_TIME = 100;


uint8_t buttonCount = 4;

uint8_t buttonPins[4] =     {};
bool buttonPressed[4] =     {};
bool buttonJustPressed[4] = {};
uint8_t debounceMillis[4] = {};

bool buttonsConfigured = false;

void onPressed(uint8_t pin);
void onReleased(uint8_t pin);

std::function<void()> onPressedCallback[4] = {nullptr, nullptr, nullptr, nullptr};
std::function<void()> onReleasedCallback[4] = {nullptr, nullptr, nullptr, nullptr};

enum ButtonState { PRESSED, RELEASED };

void configureButtons(ButtonInterface& buttonInterface) {
  buttonPins[0] = buttonInterface.bl();
  buttonPins[1] = buttonInterface.br();
  buttonPins[2] = buttonInterface.st();
  buttonPins[3] = buttonInterface.sb();
  // Set button pins to use pullup resistor
  pinMode(buttonInterface.bl(), INPUT_PULLUP);
  pinMode(buttonInterface.br(), INPUT_PULLUP);
  pinMode(buttonInterface.st(), INPUT_PULLUP);
  pinMode(buttonInterface.sb(), INPUT_PULLUP);
  buttonsConfigured = true;
}

/*
  Returns the index for the parameter pin, or -1 if not found
*/
uint8_t pinToButtonIndex(uint8_t pin) {
  for (int i = 0; i < buttonCount; i++) {
    if (buttonPins[i] == pin) {
      return i;
    }
  }
  return -1;
}

void setCallback(uint8_t pin, ButtonState buttonState, std::function<void()> callback) {
  int8_t index = pinToButtonIndex(pin);
  if (index == -1) {
    Serial.print("Failed to set callback, pin: "); Serial.print(pin); Serial.println(" not found.");
    return;
  }
  if (buttonState == RELEASED) {
    onReleasedCallback[index] = callback;
  } else {
    onPressedCallback[index] = callback;
  }
}

void resetButtonCallbacks() {
  for (int i = 0; i < buttonCount; i++) {
    onReleasedCallback[i] = nullptr;
    onPressedCallback[i] = nullptr;
  }
}

void pollButtons() {
  for (int i = 0; i < buttonCount; i++) {
    if (millis() - debounceMillis[i] > DEBOUNCE_TIME) { // debounce button interaction
      if (digitalRead(buttonPins[i]) == LOW) {      // if button is pressed
        if (!buttonPressed[i]) {              // and button wasn't pressed before
          onPressed(buttonPins[i]);
          buttonJustPressed[i] = true;        // then button was just pressed
          debounceMillis[i] = millis();
        } else {
          // if button was pressed before and is still being pressed, remove from just pressed
          buttonJustPressed[i] = false;
        }
        buttonPressed[i] = true;              // mark button as pressed
      }
      else {                                  // if button is not pressed
        //and button was pressed before
        if (buttonPressed[i]) {
          onReleased(buttonPins[i]);
          //Then clear button presses and set debounce
          buttonJustPressed[i] = false;
          buttonPressed[i] = false;
          debounceMillis[i] = millis();
        }
        // then we're done!
      }
    }
  }
}

void onPressed(uint8_t pin) {
  Serial.print("Button pressed: ");
  Serial.println(pin);
  int8_t index = pinToButtonIndex(pin);
  if (index == -1) {
    Serial.print("Unknown pin pressed: "); Serial.println(pin);
    return;
  }
  std::function<void()> callback = onPressedCallback[index];
  if (callback) {
    callback();
  }
}

void onReleased(uint8_t pin) {
  Serial.print("Button released: ");
  Serial.println(pin);
}

bool anyPressed(uint8_t pins[]) {
  int size = sizeof(pins) / sizeof(pins[0]);
  for (int i = 0; i < size; i++) {
    if (digitalRead(pins[i]) == LOW) {
      return true;
    }
  }
  return false;
}

#endif