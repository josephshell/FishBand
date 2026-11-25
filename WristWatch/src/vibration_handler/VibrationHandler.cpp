#include <vibration_handler/VibrationHandler.h>

#include <Arduino.h>

namespace
{
    constexpr uint8_t VIBRATION_MOTOR_PIN = D7;
}

VibrationHandler::VibrationHandler()
{
    pinMode(VIBRATION_MOTOR_PIN, OUTPUT);
}

void VibrationHandler::startVibration()
{
    digitalWrite(VIBRATION_MOTOR_PIN, HIGH);
}

void VibrationHandler::stopVibration()
{
    digitalWrite(VIBRATION_MOTOR_PIN, LOW);
}