#pragma once

#include <Arduino.h>
#include "navigation.h"
#include "range_sensor.h"
#include "tank_drive.h"

class RobotApp {
 public:
  RobotApp(TankDrive& drive, RangeSensor& range, Navigator& navigator,
           uint16_t sensorIntervalMs)
      : drive_(drive), range_(range), navigator_(navigator),
        sensorIntervalMs_(sensorIntervalMs) {}
  void begin();
  void update(uint32_t nowMs);

 private:
  TankDrive& drive_;
  RangeSensor& range_;
  Navigator& navigator_;
  uint16_t sensorIntervalMs_;
  uint32_t lastSenseMs_ = 0;
};

