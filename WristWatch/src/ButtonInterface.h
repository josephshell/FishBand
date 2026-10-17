#ifndef BUTTON_INTERFACE_H
#define BUTTON_INTERFACE_H

#include <Arduino.h>

class ButtonInterface {
public:
  virtual uint8_t bl() const = 0;
  virtual uint8_t br() const = 0;
  virtual uint8_t st() const = 0;
  virtual uint8_t sb() const = 0;
  
  // Destructor
  virtual ~ButtonInterface() = default;
};

#endif