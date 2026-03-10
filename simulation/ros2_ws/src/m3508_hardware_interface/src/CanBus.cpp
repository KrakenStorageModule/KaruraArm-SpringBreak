#include "m3508_hardware_interface/CanBus.hpp"
#include <rclcpp/rclcpp.hpp>
#include <fcntl.h>

namespace m3508_hardware_interface {

void CanBus::open(const std::string & iface) {
#ifdef __linux__
  sock_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (sock_ < 0) {
    throw std::runtime_error("Failed to open CAN socket: " + iface);
  }
  struct ifreq ifr;
  std::strncpy(ifr.ifr_name, iface.c_str(), IFNAMSIZ - 1);
  ioctl(sock_, SIOCGIFINDEX, &ifr);

  struct sockaddr_can addr{};
  addr.can_family  = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;
  if (bind(sock_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
    ::close(sock_); sock_ = -1;
    throw std::runtime_error("Failed to bind CAN socket");
  }

  // Set non-blocking so receiveFrame() doesn't hang
  int flags = fcntl(sock_, F_GETFL, 0);
  fcntl(sock_, F_SETFL, flags | O_NONBLOCK);

  RCLCPP_INFO(rclcpp::get_logger("CanBus"), "Opened %s", iface.c_str());
#else
  (void)iface;  // SocketCAN only available on Linux
#endif
}

void CanBus::close() {
#ifdef __linux__
  if (sock_ >= 0) { ::close(sock_); sock_ = -1; }
#endif
}

bool CanBus::sendFrame(const CanFrame & f) {
#ifdef __linux__
  struct can_frame frame{};
  frame.can_id  = f.id;
  frame.can_dlc = f.len;
  std::memcpy(frame.data, f.data, f.len);
  return write(sock_, &frame, sizeof(frame)) == sizeof(frame);
#else
  (void)f; return false;
#endif
}

bool CanBus::receiveFrame(CanFrame & f) {
#ifdef __linux__
  struct can_frame frame{};
  ssize_t n = read(sock_, &frame, sizeof(frame));
  if (n <= 0) return false;
  f.id  = frame.can_id;
  f.len = frame.can_dlc;
  std::memcpy(f.data, frame.data, frame.can_dlc);
  return true;
#else
  (void)f; return false;
#endif
}

}  // namespace m3508_hardware_interface
