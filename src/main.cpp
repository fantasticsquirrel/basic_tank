#include <Arduino.h>
#include "config.h"
#include "motor.h"
#include "navigation.h"
#include "robot_app.h"
#include "tank_drive.h"
#include "ultrasonic_sensor.h"

Motor leftMotor({config::LEFT_IN1, config::LEFT_IN2, config::LEFT_PWM,
                 config::PWM_CHANNEL_LEFT, config::INVERT_LEFT_MOTOR});
Motor rightMotor({config::RIGHT_IN1, config::RIGHT_IN2, config::RIGHT_PWM,
                  config::PWM_CHANNEL_RIGHT, config::INVERT_RIGHT_MOTOR});
TankDrive drive(leftMotor, rightMotor,
                {config::DRIVER_STANDBY, config::PWM_FREQUENCY_HZ,
                 config::PWM_RESOLUTION_BITS, config::RAMP_STEP,
                 config::RAMP_INTERVAL_MS,
                 {config::CRUISE_SPEED, config::REVERSE_SPEED, config::TURN_SPEED}});
UltrasonicSensor rangeSensor({config::ULTRASONIC_TRIGGER, config::ULTRASONIC_ECHO,
                              config::SONAR_SAMPLES, config::ECHO_TIMEOUT_US,
                              2.0F, 400.0F});
Navigator navigator({config::STOP_DISTANCE_CM, config::CLEAR_DISTANCE_CM,
                     config::MAX_INVALID_READINGS, config::STOP_PAUSE_MS,
                     config::REVERSE_TIME_MS, config::TURN_TIME_MS,
                     config::RECOVERY_TIME_MS});
RobotApp robot(drive, rangeSensor, navigator, config::SENSOR_INTERVAL_MS);

void setup() {
  Serial.begin(115200);
  robot.begin();
}

void loop() { robot.update(millis()); }

