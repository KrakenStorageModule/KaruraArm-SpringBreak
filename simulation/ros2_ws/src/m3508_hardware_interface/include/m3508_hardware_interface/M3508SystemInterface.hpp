#pragma once

#include "m3508_hardware_interface/hardware_config.hpp"
#include <vector>
#include <string>
#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/handle.hpp"
#include "hardware_interface/hardware_info.hpp"
#include "hardware_interface/types/hardware_interface_type_values.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/state.hpp"
#include "m3508_hardware_interface/CanBus.hpp"
#include "m3508_hardware_interface/M3508Motor.hpp"
#include "m3508_hardware_interface/AMT20Bridge.hpp"

namespace m3508_hardware_interface
{

class M3508SystemInterface : public hardware_interface::SystemInterface
{
public:
  hardware_interface::CallbackReturn on_init(
    const hardware_interface::HardwareInfo & info) override;

  hardware_interface::CallbackReturn on_configure(
    const rclcpp_lifecycle::State & previous_state) override;

  hardware_interface::CallbackReturn on_activate(
    const rclcpp_lifecycle::State & previous_state) override;

  hardware_interface::CallbackReturn on_deactivate(
    const rclcpp_lifecycle::State & previous_state) override;

  std::vector<hardware_interface::StateInterface> export_state_interfaces() override;

  std::vector<hardware_interface::CommandInterface> export_command_interfaces() override;

  hardware_interface::return_type read(
    const rclcpp::Time & time,
    const rclcpp::Duration & period) override;

  hardware_interface::return_type write(
    const rclcpp::Time & time,
    const rclcpp::Duration & period) override;

private:
  std::vector<double> hw_positions_;   // Current joint positions (rad)
  std::vector<double> hw_velocities_;  // Current joint velocities (rad/s)
  std::vector<double> hw_commands_;    // Commanded positions from controller
  // Hardware communication objects
  CanBus    can_bus_;
  AMT20Bridge encoder_bridge_;
  bool encoder_ok_ = true;

  // PID gains (loaded from URDF parameters on Day 7)
  double Kp_ = 500.0;   // Proportional gain — start low for hardware
  double Ki_ = 0.0;     // Integral gain (disabled for now)
  double Kd_ = 10.0;    // Derivative gain
  std::vector<double> pid_integral_;
  std::vector<double> pid_prev_error_;

};

}  // namespace m3508_hardware_interface
