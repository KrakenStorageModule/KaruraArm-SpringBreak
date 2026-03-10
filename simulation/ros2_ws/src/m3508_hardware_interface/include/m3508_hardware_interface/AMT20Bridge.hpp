#pragma once
#include "m3508_hardware_interface/hardware_config.hpp"
#include <vector>
#include <string>
#include <cmath>

namespace m3508_hardware_interface {

class AMT20Bridge {
public:
  // In simulation: returns zeros. In hardware mode: reads from serial port.
  bool initialize(const std::string & port = "/dev/ttyACM0",
                  int baud = 115200);
  void close();

  // Returns joint positions in radians for all 6 joints.
  // Returns false if read failed (call emergency stop if false).
  bool getPositions(std::vector<double> & positions_rad);
    
  //encoder safety
  bool encoder_ok_ = true;
  // Converts AMT20 12-bit count (0–4095) to radians
  static double countsToRadians(int counts) {
    return (static_cast<double>(counts) / 4096.0) * 2.0 * M_PI;
  }

private:
#if HARDWARE_ENABLED
  int serial_fd_ = -1;
  bool parseCSVLine(const std::string & line,
                    std::vector<double> & out);
#endif
};

}  // namespace m3508_hardware_interface

