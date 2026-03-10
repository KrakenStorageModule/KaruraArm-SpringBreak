#pragma once
// Set to 1 when real hardware (MCP2515 + AMT20) is connected.
// Set to 0 for simulation (echo mode, no hardware required).
// Rebuild after changing: colcon build --packages-select m3508_hardware_interface
#define HARDWARE_ENABLED 0
