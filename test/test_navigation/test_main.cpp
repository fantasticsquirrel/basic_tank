#include <unity.h>
#include "drive_model.h"
#include "navigation.h"

static Navigator makeNavigator() { return Navigator({25, 34, 3, 100, 400, 500, 200}); }

void test_cruises_when_clear() {
  auto nav = makeNavigator();
  TEST_ASSERT_EQUAL_INT((int)Motion::Forward, (int)nav.update(100, true, 0).motion);
}

void test_obstacle_runs_escape_sequence() {
  auto nav = makeNavigator();
  TEST_ASSERT_EQUAL_INT((int)Motion::Stop, (int)nav.update(20, true, 10).motion);
  TEST_ASSERT_EQUAL_INT((int)Motion::Reverse, (int)nav.update(20, true, 110).motion);
  TEST_ASSERT_EQUAL_INT((int)Motion::TurnLeft, (int)nav.update(20, true, 510).motion);
  TEST_ASSERT_EQUAL_INT((int)Motion::Forward, (int)nav.update(50, true, 1010).motion);
  TEST_ASSERT_EQUAL_INT((int)NavPhase::Cruising, (int)nav.update(50, true, 1210).phase);
}

void test_invalid_sensor_fails_safe_and_recovers() {
  auto nav = makeNavigator();
  nav.update(0, false, 10); nav.update(0, false, 20);
  auto failed = nav.update(0, false, 30);
  TEST_ASSERT_EQUAL_INT((int)NavPhase::Fault, (int)failed.phase);
  TEST_ASSERT_EQUAL_INT((int)Motion::Stop, (int)failed.motion);
  TEST_ASSERT_EQUAL_INT((int)NavPhase::Pause, (int)nav.update(50, true, 40).phase);
}

void test_turn_direction_alternates() {
  auto nav = makeNavigator();
  nav.update(10, true, 0); nav.update(10, true, 100); nav.update(10, true, 500);
  TEST_ASSERT_EQUAL_INT((int)Motion::TurnLeft, (int)nav.update(10, true, 501).motion);
  nav.update(50, true, 1000); nav.update(50, true, 1200);
  nav.update(10, true, 1210); nav.update(10, true, 1310); nav.update(10, true, 1710);
  TEST_ASSERT_EQUAL_INT((int)Motion::TurnRight, (int)nav.update(10, true, 1711).motion);
}

void test_motion_maps_to_independent_treads() {
  const DriveSpeeds speeds{135, 125, 140};
  WheelSpeeds wheels = speedsForMotion(Motion::TurnLeft, speeds);
  TEST_ASSERT_EQUAL_INT(-140, wheels.left);
  TEST_ASSERT_EQUAL_INT(140, wheels.right);
  wheels = speedsForMotion(Motion::Reverse, speeds);
  TEST_ASSERT_EQUAL_INT(-125, wheels.left);
  TEST_ASSERT_EQUAL_INT(-125, wheels.right);
}

void test_ramp_never_overshoots() {
  TEST_ASSERT_EQUAL_INT(8, rampToward(0, 100, 8));
  TEST_ASSERT_EQUAL_INT(100, rampToward(98, 100, 8));
  TEST_ASSERT_EQUAL_INT(-8, rampToward(0, -100, 8));
  TEST_ASSERT_EQUAL_INT(-100, rampToward(-98, -100, 8));
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_cruises_when_clear);
  RUN_TEST(test_obstacle_runs_escape_sequence);
  RUN_TEST(test_invalid_sensor_fails_safe_and_recovers);
  RUN_TEST(test_turn_direction_alternates);
  RUN_TEST(test_motion_maps_to_independent_treads);
  RUN_TEST(test_ramp_never_overshoots);
  return UNITY_END();
}
