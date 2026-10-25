#include <BatteryManagement.h>

#define BATTERY_PIN D0

const float PIN_VALUE_AT_100_PERCENT = 3307.5;
const uint16_t PIN_VALUE_AT_20_PERCENT = 2520;
const float BATTERY_VALUE_SLOPE = 100.0 / (PIN_VALUE_AT_100_PERCENT - PIN_VALUE_AT_20_PERCENT);

bool initialized = false;

namespace
{
    uint16_t getBatteryPinVal()
    {
        return analogRead(BATTERY_PIN);
    }
}

namespace BatteryManagement
{
    void initialize()
    {
        pinMode(BATTERY_PIN, INPUT);
    }

    int8_t getBatteryLevel()
    {
        uint16_t pinVal = getBatteryPinVal();
        int8_t batteryLevel = BATTERY_VALUE_SLOPE * pinVal - 320;
        if (batteryLevel > 100)
        {
            Serial.print("Battery read more than maximum of ");
            Serial.print(PIN_VALUE_AT_100_PERCENT);
            Serial.print(": ");
            Serial.println(pinVal);
            Serial.print("Battery Level read as: ");
            Serial.println(batteryLevel);
            Serial.println("Returning battery level of -1");
            batteryLevel = -1;
        }
        if (batteryLevel < 0)
        {
            Serial.print("Battery read less safe minimum of ");
            Serial.print(PIN_VALUE_AT_20_PERCENT);
            Serial.print(": ");
            Serial.println(pinVal);
            Serial.print("Battery Level read as: ");
            Serial.println(batteryLevel);
            Serial.println("Returning battery level of -1");
            batteryLevel = -1;
        }
        return batteryLevel;
    }
}