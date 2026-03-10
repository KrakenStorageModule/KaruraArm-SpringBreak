#include "m3508_hardware_interface/M3508Motor.hpp"
#include <cstring>

namespace m3508_hardware_interface {

CanFrame M3508MotorBank::buildCommandFrame14(
  int16_t m1, int16_t m2, int16_t m3, int16_t m4)
{
  CanFrame f;
  f.id  = 0x200;
  f.len = 8;
  // Each motor: high byte first, then low byte
  f.data[0] = (m1 >> 8) & 0xFF;  f.data[1] = m1 & 0xFF;
  f.data[2] = (m2 >> 8) & 0xFF;  f.data[3] = m2 & 0xFF;
  f.data[4] = (m3 >> 8) & 0xFF;  f.data[5] = m3 & 0xFF;
  f.data[6] = (m4 >> 8) & 0xFF;  f.data[7] = m4 & 0xFF;
  return f;
}

CanFrame M3508MotorBank::buildCommandFrame58(
  int16_t m5, int16_t m6, int16_t m7, int16_t m8)
{
  CanFrame f;
  f.id  = 0x1FF;
  f.len = 8;
  f.data[0] = (m5 >> 8) & 0xFF;  f.data[1] = m5 & 0xFF;
  f.data[2] = (m6 >> 8) & 0xFF;  f.data[3] = m6 & 0xFF;
  f.data[4] = (m7 >> 8) & 0xFF;  f.data[5] = m7 & 0xFF;
  f.data[6] = (m8 >> 8) & 0xFF;  f.data[7] = m8 & 0xFF;
  return f;
}

M3508Feedback M3508MotorBank::parseFeedback(const CanFrame & frame) {
  M3508Feedback fb;
  fb.raw_angle      = (uint16_t)(frame.data[0] << 8) | frame.data[1];
  fb.rpm            = (int16_t) (frame.data[2] << 8) | frame.data[3];
  fb.actual_current = (int16_t) (frame.data[4] << 8) | frame.data[5];
  fb.temperature    = frame.data[6];
  return fb;
}

}  // namespace m3508_hardware_interface

