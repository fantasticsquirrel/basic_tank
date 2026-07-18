#pragma once

#include <stdint.h>

namespace config {
// ESP32 DevKit V1 -> TB6612FNG. These avoid boot-strapping pins.
constexpr uint8_t LEFT_IN1 = 16;
constexpr uint8_t LEFT_IN2 = 17;
constexpr uint8_t LEFT_PWM = 18;
constexpr uint8_t RIGHT_IN1 = 19;
constexpr uint8_t RIGHT_IN2 = 21;
constexpr uint8_t RIGHT_PWM = 22;
constexpr uint8_t DRIVER_STANDBY = 23;

constexpr uint8_t ULTRASONIC_TRIGGER = 25;
constexpr uint8_t ULTRASONIC_ECHO = 34;  // input-only is ideal here

constexpr uint8_t PWM_CHANNEL_LEFT = 0;
constexpr uint8_t PWM_CHANNEL_RIGHT = 1;
constexpr uint16_t PWM_FREQUENCY_HZ = 18000;
constexpr uint8_t PWM_RESOLUTION_BITS = 8;

// Conservative starting values for an N20 drivetrain. Raise only after a
// tread-off-the-ground test; actual speed depends heavily on gearbox ratio.
constexpr int CRUISE_SPEED = 135;       // 0..255
constexpr int REVERSE_SPEED = 125;
constexpr int TURN_SPEED = 140;
constexpr int RAMP_STEP = 8;
constexpr uint16_t RAMP_INTERVAL_MS = 12;

constexpr float STOP_DISTANCE_CM = 25.0F;
constexpr float CLEAR_DISTANCE_CM = 34.0F;  // hysteresis prevents twitching
constexpr uint8_t SONAR_SAMPLES = 5;
constexpr uint32_t ECHO_TIMEOUT_US = 25000; // about 4 m maximum
constexpr uint16_t SENSOR_INTERVAL_MS = 70;
constexpr uint8_t MAX_INVALID_READINGS = 4;

constexpr uint16_t STOP_PAUSE_MS = 120;
constexpr uint16_t REVERSE_TIME_MS = 420;
constexpr uint16_t TURN_TIME_MS = 520;
constexpr uint16_t RECOVERY_TIME_MS = 180;

// Flip either one if that tread runs backward when FORWARD is requested.
constexpr bool INVERT_LEFT_MOTOR = false;
constexpr bool INVERT_RIGHT_MOTOR = true;
}  // namespace config
