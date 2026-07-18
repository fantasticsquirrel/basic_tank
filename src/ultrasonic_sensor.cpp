#include "ultrasonic_sensor.h"

#include <algorithm>
#include <cmath>

void UltrasonicSensor::begin() {
  pinMode(config_.triggerPin, OUTPUT);
  pinMode(config_.echoPin, INPUT);
  digitalWrite(config_.triggerPin, LOW);
}

float UltrasonicSensor::readOnce() {
  digitalWrite(config_.triggerPin, LOW);
  delayMicroseconds(3);
  digitalWrite(config_.triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(config_.triggerPin, LOW);
  const uint32_t duration = pulseIn(config_.echoPin, HIGH, config_.echoTimeoutUs);
  if (duration == 0) return NAN;
  const float distance = duration * 0.0343F / 2.0F;
  return distance >= config_.minimumCm && distance <= config_.maximumCm ? distance : NAN;
}

RangeReading UltrasonicSensor::read() {
  constexpr uint8_t MAX_SAMPLES = 9;
  float values[MAX_SAMPLES];
  const uint8_t requested = constrain(config_.samples, 1, MAX_SAMPLES);
  uint8_t count = 0;
  for (uint8_t i = 0; i < requested; ++i) {
    const float value = readOnce();
    if (!std::isnan(value)) values[count++] = value;
    if (i + 1 < requested) delay(8);
  }
  if (count < requested / 2 + 1) return {0.0F, false};
  std::sort(values, values + count);
  return {values[count / 2], true};
}

