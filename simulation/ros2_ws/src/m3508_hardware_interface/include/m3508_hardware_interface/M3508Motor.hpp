#pragma once
#include <cstdint>
#include <cmath>
#include <array>
#include "m3508_hardware_interface/CanBus.hpp"

namespace m3508_hardware_interface {

struct M3508Feedback {
  uint16_t raw_angle;     // 0–8191 (14-bit, relative)
  int16_t  rpm;           // rotations per minute
  int16_t  actual_current;
  uint8_t  temperature;   // degrees Celsius
};

class M3508MotorBank {
public:
  // Build and send a command frame for motors 1–4 (CAN ID 0x200)
  // or motors 5–8 (CAN ID 0x1FF).
  // motor_index: 0-based (0=motor1, 1=motor2, ...)
  // current: -16384 to +16384
  static CanFrame buildCommandFrame14(
    int16_t m1, int16_t m2, int16_t m3, int16_t m4);
  static CanFrame buildCommandFrame58(
    int16_t m5, int16_t m6, int16_t m7, int16_t m8);

  // Parse a feedback frame from motor at CAN ID (0x201 + motor_id - 1)
  static M3508Feedback parseFeedback(const CanFrame & frame);

  // Convert raw 14-bit angle to radians
  static double rawAngleToRadians(uint16_t raw) {
    return (static_cast<double>(raw) / 8191.0) * 2.0 * M_PI;
  }

  // Clamp a value to [min, max]
  static int16_t clampCurrent(double value) {
    if (value >  16384.0) return  16384;
    if (value < -16384.0) return -16384;
    return static_cast<int16_t>(value);
  }
};

}  // namespace m3508_hardware_interface

