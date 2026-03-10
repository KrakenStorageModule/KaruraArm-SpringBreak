#include "m3508_hardware_interface/M3508SystemInterface.hpp"
#include <cmath>

namespace m3508_hardware_interface {

hardware_interface::CallbackReturn
M3508SystemInterface::on_init(const hardware_interface::HardwareInfo &info) {
  if (hardware_interface::SystemInterface::on_init(info) !=
      hardware_interface::CallbackReturn::SUCCESS) {
    return hardware_interface::CallbackReturn::ERROR;
  }

  hw_positions_.assign(info_.joints.size(), 0.0);
  hw_velocities_.assign(info_.joints.size(), 0.0);
  hw_commands_.assign(info_.joints.size(), 0.0);

  RCLCPP_INFO(rclcpp::get_logger("M3508SystemInterface"),
              "Initialized with %zu joints [HARDWARE_ENABLED=%d]",
              info_.joints.size(), HARDWARE_ENABLED);

  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn M3508SystemInterface::on_configure(
    const rclcpp_lifecycle::State & /*previous_state*/) {
  std::fill(hw_positions_.begin(), hw_positions_.end(), 0.0);
  std::fill(hw_velocities_.begin(), hw_velocities_.end(), 0.0);
  std::fill(hw_commands_.begin(), hw_commands_.end(), 0.0);

  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn M3508SystemInterface::on_activate(
    const rclcpp_lifecycle::State & /*previous_state*/) {
  for (size_t i = 0; i < info_.joints.size(); i++) {
    auto &joint = info_.joints[i];
    RCLCPP_INFO(rclcpp::get_logger("M3508SystemInterface"),
                "Joint %s at %.3f rad on activate", joint.name.c_str(),
                hw_positions_[i]);
  }

  hw_commands_ = hw_positions_;
  std::fill(pid_integral_.begin(), pid_integral_.end(), 0.0);
  std::fill(pid_prev_error_.begin(), pid_prev_error_.end(), 0.0);

  RCLCPP_INFO(rclcpp::get_logger("M3508SystemInterface"),
              "Hardware interface activated safely");
  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn M3508SystemInterface::on_deactivate(
    const rclcpp_lifecycle::State & /*previous_state*/) {
  RCLCPP_INFO(rclcpp::get_logger("M3508SystemInterface"), "Deactivated");
  return hardware_interface::CallbackReturn::SUCCESS;
}

std::vector<hardware_interface::StateInterface>
M3508SystemInterface::export_state_interfaces() {
  std::vector<hardware_interface::StateInterface> si;
  for (size_t i = 0; i < info_.joints.size(); i++) {
    si.emplace_back(info_.joints[i].name, hardware_interface::HW_IF_POSITION,
                    &hw_positions_[i]);
    si.emplace_back(info_.joints[i].name, hardware_interface::HW_IF_VELOCITY,
                    &hw_velocities_[i]);
  }
  return si;
}

std::vector<hardware_interface::CommandInterface>
M3508SystemInterface::export_command_interfaces() {
  std::vector<hardware_interface::CommandInterface> ci;
  for (size_t i = 0; i < info_.joints.size(); i++) {
    ci.emplace_back(info_.joints[i].name, hardware_interface::HW_IF_POSITION,
                    &hw_commands_[i]);
  }
  return ci;
}

hardware_interface::return_type
M3508SystemInterface::read(const rclcpp::Time & /*time*/,
                           const rclcpp::Duration & /*period*/) {
#if HARDWARE_ENABLED
  if (!encoder_bridge_.getPositions(hw_positions_)) {
    encoder_ok_ = false;
    RCLCPP_WARN_THROTTLE(rclcpp::get_logger("M3508SystemInterface"),
                         *rclcpp::Clock::make_shared(), 1000,
                         "Encoder read failed");
  } else {
    encoder_ok_ = true;
  }
#else
  for (size_t i = 0; i < hw_positions_.size(); i++) {
    hw_positions_[i] = hw_commands_[i];
    hw_velocities_[i] = 0.0;
  }
#endif
  return hardware_interface::return_type::OK;
}

hardware_interface::return_type
M3508SystemInterface::write(const rclcpp::Time & /*time*/,
                            const rclcpp::Duration & /*period*/) {
#if HARDWARE_ENABLED
  // TODO Day 7: Implement PID control loop here
  if (!encoder_ok_) {
    CanFrame safe = M3508MotorBank::buildCommandFrame14(0, 0, 0, 0);
    can_bus_.sendFrame(safe);
    safe = M3508MotorBank::buildCommandFrame58(0, 0, 0, 0);
    can_bus_.sendFrame(safe);
    RCLCPP_ERROR_THROTTLE(rclcpp::get_logger("M3508SystemInterface"),
                          *rclcpp::Clock::make_shared(), 1000,
                          "Encoder failure — motors zeroed for safety");
  }
#endif
  return hardware_interface::return_type::OK;
}

} // namespace m3508_hardware_interface

#include "pluginlib/class_list_macros.hpp"
PLUGINLIB_EXPORT_CLASS(m3508_hardware_interface::M3508SystemInterface,
                       hardware_interface::SystemInterface)
