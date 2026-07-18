#pragma once

struct RangeReading {
  float centimeters;
  bool valid;
};

class RangeSensor {
 public:
  virtual ~RangeSensor() = default;
  virtual void begin() = 0;
  virtual RangeReading read() = 0;
};

