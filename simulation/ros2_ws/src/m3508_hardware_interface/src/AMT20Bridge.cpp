#include "m3508_hardware_interface/AMT20Bridge.hpp"
#include "m3508_hardware_interface/M3508SystemInterface.hpp"
#include <rclcpp/rclcpp.hpp>

#if HARDWARE_ENABLED
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#endif

namespace m3508_hardware_interface {

bool AMT20Bridge::initialize(const std::string & port, int /*baud*/) {
#if HARDWARE_ENABLED
  // Open serial port
  serial_fd_ = ::open(port.c_str(), O_RDONLY | O_NOCTTY | O_NONBLOCK);
  if (serial_fd_ < 0) {
    RCLCPP_ERROR(rclcpp::get_logger("AMT20Bridge"),
      "Cannot open serial port %s", port.c_str());
    return false;
  }
  struct termios tty{};
  cfsetispeed(&tty, B115200);
  tty.c_cflag = CS8 | CREAD | CLOCAL;
  tcsetattr(serial_fd_, TCSANOW, &tty);
  RCLCPP_INFO(rclcpp::get_logger("AMT20Bridge"),
    "Opened encoder serial port %s", port.c_str());
  return true;
#else
  (void)port;
  RCLCPP_INFO(rclcpp::get_logger("AMT20Bridge"),
    "AMT20Bridge running in SIMULATION mode");
  return true;
#endif
}

void AMT20Bridge::close() {
#if HARDWARE_ENABLED
  if (serial_fd_ >= 0) { ::close(serial_fd_); serial_fd_ = -1; }
#endif
}

bool AMT20Bridge::getPositions(std::vector<double> & positions_rad) {
  if (positions_rad.size() < 6) positions_rad.resize(6, 0.0);
#if HARDWARE_ENABLED
  // Read a CSV line from serial: "45.2,90.1,135.0,0.0,270.5,180.3\n"
  char buf[128];
  ssize_t n = read(serial_fd_, buf, sizeof(buf)-1);
  if (n <= 0) return false;
  buf[n] = '\0';
  return parseCSVLine(std::string(buf), positions_rad);
#else
  // In simulation, return zeros (the echo in read() handles state)
  std::fill(positions_rad.begin(), positions_rad.end(), 0.0);
  return true;
#endif
}

#if HARDWARE_ENABLED
bool AMT20Bridge::parseCSVLine(
  const std::string & line, std::vector<double> & out) {
  std::istringstream ss(line);
  std::string token;
  size_t i = 0;
  while (std::getline(ss, token, ',') && i < out.size()) {
    try {
      double deg = std::stod(token);
      out[i++] = deg * M_PI / 180.0;  // degrees → radians
    } catch (...) { return false; }
  }
  return i == out.size();
}
#endif

}  // namespace m3508_hardware_interface

