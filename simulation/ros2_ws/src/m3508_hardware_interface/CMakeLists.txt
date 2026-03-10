cmake_minimum_required(VERSION 3.8)
project(m3508_hardware_interface)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID
MATCHES "Clang")
add_compile_options(-Wall -Wextra -Wpedantic)
endif()
find_package(ament_cmake REQUIRED)
find_package(hardware_interface REQUIRED)
find_package(pluginlib REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
add_library(m3508_hardware_interface SHARED
src/M3508SystemInterface.cpp
src/CanBus.cpp
src/M3508Motor.cpp
src/AMT20Bridge.cpp
)
target_include_directories(m3508_hardware_interface PUBLIC
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include>
)
ament_target_dependencies(m3508_hardware_interface
hardware_interface pluginlib rclcpp rclcpp_lifecycle
)
pluginlib_export_plugin_description_file(
hardware_interface m3508_hardware_interface.xml)
install(TARGETS m3508_hardware_interface
EXPORT m3508_hardware_interface
ARCHIVE DESTINATION lib
LIBRARY DESTINATION lib
RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)
install(FILES m3508_hardware_interface.xml
DESTINATION share/${PROJECT_NAME}
)
ament_package()