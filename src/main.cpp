#include <Arduino.h>
#include <algorithm>
#include <cmath>
#include "config.h"
#include "navigation.h"

namespace {
Navigator navigator({config::STOP_DISTANCE_CM, config::CLEAR_DISTANCE_CM,
                     config::MAX_INVALID_READINGS, config::STOP_PAUSE_MS,
                     config::REVERSE_TIME_MS, config::TURN_TIME_MS,
                     config::RECOVERY_TIME_MS});
int currentLeft = 0;
int currentRight = 0;
int targetLeft = 0;
int targetRight = 0;
uint32_t lastRampMs = 0;
uint32_t lastSenseMs = 0;

void setMotorPins(uint8_t in1, uint8_t in2, uint8_t channel, int speed, bool inverted) {
  int signedSpeed = inverted ? -speed : speed;
  digitalWrite(in1, signedSpeed >= 0 ? HIGH : LOW);
  digitalWrite(in2, signedSpeed >= 0 ? LOW : HIGH);
  ledcWrite(channel, abs(signedSpeed));
}

int approach(int current, int target) {
  if (current < target) return std::min(current + config::RAMP_STEP, target);
  if (current > target) return std::max(current - config::RAMP_STEP, target);
  return current;
}

void updateMotors(uint32_t nowMs) {
  if (nowMs - lastRampMs < config::RAMP_INTERVAL_MS) return;
  lastRampMs = nowMs;
  currentLeft = approach(currentLeft, targetLeft);
  currentRight = approach(currentRight, targetRight);
  setMotorPins(config::LEFT_IN1, config::LEFT_IN2, config::PWM_CHANNEL_LEFT,
               currentLeft, config::INVERT_LEFT_MOTOR);
  setMotorPins(config::RIGHT_IN1, config::RIGHT_IN2, config::PWM_CHANNEL_RIGHT,
               currentRight, config::INVERT_RIGHT_MOTOR);
}

void requestMotion(Motion motion) {
  switch (motion) {
    case Motion::Forward: targetLeft = targetRight = config::CRUISE_SPEED; break;
    case Motion::Reverse: targetLeft = targetRight = -config::REVERSE_SPEED; break;
    case Motion::TurnLeft: targetLeft = -config::TURN_SPEED; targetRight = config::TURN_SPEED; break;
    case Motion::TurnRight: targetLeft = config::TURN_SPEED; targetRight = -config::TURN_SPEED; break;
    default: targetLeft = targetRight = 0; break;
  }
}

float singleDistanceCm() {
  digitalWrite(config::ULTRASONIC_TRIGGER, LOW);
  delayMicroseconds(3);
  digitalWrite(config::ULTRASONIC_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(config::ULTRASONIC_TRIGGER, LOW);
  const uint32_t duration = pulseIn(config::ULTRASONIC_ECHO, HIGH, config::ECHO_TIMEOUT_US);
  if (duration == 0) return NAN;
  const float distance = duration * 0.0343F / 2.0F;
  return (distance >= 2.0F && distance <= 400.0F) ? distance : NAN;
}

float medianDistanceCm() {
  float values[config::SONAR_SAMPLES];
  uint8_t count = 0;
  for (uint8_t i = 0; i < config::SONAR_SAMPLES; ++i) {
    const float reading = singleDistanceCm();
    if (!std::isnan(reading)) values[count++] = reading;
    delay(8);
  }
  if (count < (config::SONAR_SAMPLES / 2 + 1)) return NAN;
  std::sort(values, values + count);
  return values[count / 2];
}
}  // namespace

void setup() {
  Serial.begin(115200);
  pinMode(config::LEFT_IN1, OUTPUT); pinMode(config::LEFT_IN2, OUTPUT);
  pinMode(config::RIGHT_IN1, OUTPUT); pinMode(config::RIGHT_IN2, OUTPUT);
  pinMode(config::DRIVER_STANDBY, OUTPUT);
  pinMode(config::ULTRASONIC_TRIGGER, OUTPUT);
  pinMode(config::ULTRASONIC_ECHO, INPUT);
  ledcSetup(config::PWM_CHANNEL_LEFT, config::PWM_FREQUENCY_HZ, config::PWM_RESOLUTION_BITS);
  ledcSetup(config::PWM_CHANNEL_RIGHT, config::PWM_FREQUENCY_HZ, config::PWM_RESOLUTION_BITS);
  ledcAttachPin(config::LEFT_PWM, config::PWM_CHANNEL_LEFT);
  ledcAttachPin(config::RIGHT_PWM, config::PWM_CHANNEL_RIGHT);
  digitalWrite(config::DRIVER_STANDBY, HIGH);
  navigator.reset(millis());
  Serial.println("basic_tank ready; lift treads for first test");
}

void loop() {
  const uint32_t now = millis();
  updateMotors(now);
  if (now - lastSenseMs < config::SENSOR_INTERVAL_MS) return;
  lastSenseMs = now;

  const float distance = medianDistanceCm();
  const bool valid = !std::isnan(distance);
  const NavOutput output = navigator.update(distance, valid, millis());
  requestMotion(output.motion);
  Serial.printf("range=%s", valid ? "" : "invalid");
  if (valid) Serial.printf("%.1fcm", distance);
  Serial.printf(" phase=%s motion=%s%s\n", phaseName(output.phase),
                motionName(output.motion), output.changed ? " *" : "");
}

