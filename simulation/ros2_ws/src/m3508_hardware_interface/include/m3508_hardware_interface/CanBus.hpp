#pragma once
#include <cstdint>
#include <string>
#include <stdexcept>

// Linux SocketCAN headers — only available on Linux
#ifdef __linux__
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>
#endif

namespace m3508_hardware_interface {

struct CanFrame {
  uint32_t id;
  uint8_t  len;
  uint8_t  data[8];
};

class CanBus {
public:
  // Open the SocketCAN interface (e.g., "can0")
  void open(const std::string & interface_name);
  void close();
  bool isOpen() const { return sock_ >= 0; }

  // Send a CAN frame
  bool sendFrame(const CanFrame & frame);

  // Receive a CAN frame (non-blocking, returns false if no frame available)
  bool receiveFrame(CanFrame & frame);

private:
  int sock_ = -1;
};

}  // namespace m3508_hardware_interface

