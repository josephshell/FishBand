#ifndef BATTERY_MANAGEMENT_H
#define BATTERY_MANAGEMENT_H

#include <Arduino.h>

namespace BatteryManagement
{
    /**
     * Returns an integer between -1 and 100, where 100% is a fully charged battery, and 0% is a battery at 20% of max voltage. Return value -1 is reserved for when a value read is
     * out of the expected range.
     *
     * The battery is considered fully charged at an pin value of 3307.5, which is approximately 2.1V, and considered empty at an pin value of 2510, which is approximately 1.6V.
     * Expected battery is a 3.7V Lipo battery, measured using a 50/50 voltage divider. The operating voltage is expected to be 4.2V to 3.2V, so we consider 3.7V to be a safe minimum charge.
     */
    int8_t getBatteryLevel();

    /**
     * Initializes the battery service, setting up the input pin and
     */
    void initialize();

}
#endif
