#ifndef ESP32_C3_BUTTON_IMPL_H
#define ESP32_C3_BUTTON_IMPL_H

#include <Arduino.h>
#include <ButtonInterface.h>

class Esp32C3ButtonImpl : public ButtonInterface {
public:
  uint8_t bl() const override {
    return D3;
  }
  uint8_t br() const override {
    return D2;
  }
  uint8_t st() const override {
    return D1;
  }
  uint8_t sb() const override {
    return D0;
  }
};

#endif