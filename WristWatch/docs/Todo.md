# TODO
## Time Keeping
- I want to set the time on the watch when it's out of date

##
## Battery
1.
## Alarms
### Creating functional Alarms
1. First, I want a serial monitor notification that an alarm's time has occurred
  1. Calculate timestamp of next alarm
  2. Compare timestamp to current time
  3. Set active alarm and change to sound_alarm state
2. Second, for testing, I want to blink an LED when an alarm occurs
3. Third, I want to wake the device from deep sleep when the alarm occurs (Use rtc's alarm function?)
4. When coin motor arrives, I want to to replace LED with MOSFET, coin motor, and introduce power supply
### Alarm CRUD
1. Alarms can be created already
2. I want to cancel creating an alarm, right now if you start creating an alarm, you have to complete it
3. I want selecting an alarm in the "ls Alarms" menu to show DELETE and EDIT options
4. I want selecting DELETE to destroy an alarm
5. I want selecting EDIT to reenter the alarm builder and update the existing alarm
## Alarm Details
1. Alarms should be fully visible in "ls Alarms" menu
## Deep Sleep
- I want the device to go into deep sleep after 15 continuous seconds on the  time screen
- I want the device to go into deep sleep after 15 seconds on the menu screen
  - This timer should reset on button press
- I want the device to wake when any button is pressed
- I want the device to wake on a timed schedule when any alarm is present

## Tech Debt
1. `changeToState()` should only be called at the end of the loop. Make sure that only one state change happens per loop