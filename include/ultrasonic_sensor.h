#pragma once

#include <Arduino.h>
#include "range_sensor.h"

struct UltrasonicConfig {
  uint8_t triggerPin;
  uint8_t echoPin;
  uint8_t samples;
  uint32_t echoTimeoutUs;
  float minimumCm;
  float maximumCm;
};

class UltrasonicSensor : public RangeSensor {
 public:
  explicit UltrasonicSensor(UltrasonicConfig config) : config_(config) {}
  void begin() override;
  RangeReading read() override;

 private:
  float readOnce();
  UltrasonicConfig config_;
};

