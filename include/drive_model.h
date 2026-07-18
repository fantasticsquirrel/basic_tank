#pragma once

#include "navigation.h"

struct WheelSpeeds {
  int left;
  int right;
};

struct DriveSpeeds {
  int cruise;
  int reverse;
  int turn;
};

WheelSpeeds speedsForMotion(Motion motion, const DriveSpeeds& speeds);
int rampToward(int current, int target, int step);

