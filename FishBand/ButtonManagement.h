#ifndef BUTTON_MANAGEMENT_H
#define BUTTON_MANAGEMENT_H

#include <Arduino.h>

/**
 * Millisecond time to wait between reading button states to insure
 * that bounces in the button press do not register as multiple presses.
*/
const uint8_t DEBOUNCE_TIME = 100;

const uint8_t BL = D3; // Bottom left button
const uint8_t BR = D2; // Bottom right button
const uint8_t ST = D1; // Side top button
const uint8_t SB = D0; // Side bottom button

uint8_t buttonCount = 4;

uint8_t buttonPins[4] =     { BL,      BR,    ST,    SB    };
bool buttonPressed[4] =     { false,   false, false, false };
bool buttonJustPressed[4] = { false,   false, false, false };
uint8_t debounceMillis[4] = { 0,       0,     0,     0     };

void onPressed(uint8_t pin);
void onReleased(uint8_t pin);

std::function<void()> onPressedCallback[4] = {nullptr, nullptr, nullptr, nullptr};
std::function<void()> onReleasedCallback[4] = {nullptr, nullptr, nullptr, nullptr};

enum ButtonState { PRESSED, RELEASED };

uint8_t pinToButtonIndex(uint8_t pin) {
  switch (pin) {
    case BL:
      return 0;
    case BR:
      return 1;
    case ST:
      return 2;
    case SB:
      return 3;
    default:
      Serial.print("ERROR: UNKNOWN PIN ");
      Serial.println(pin);
  }
}

void setCallback(uint8_t pin, ButtonState buttonState, std::function<void()> callback) {
  if (buttonState == RELEASED) {
    onReleasedCallback[pinToButtonIndex(pin)] = callback;
  } else {
    onPressedCallback[pinToButtonIndex(pin)] = callback;
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
  std::function<void()> callback = onPressedCallback[pinToButtonIndex(pin)];
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