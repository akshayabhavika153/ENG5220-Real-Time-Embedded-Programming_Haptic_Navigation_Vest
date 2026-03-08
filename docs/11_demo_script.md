# Demo Script: Haptic Navigation Vest

## Overview
This document provides step-by-step instructions for demonstrating the Haptic Navigation Vest system.

## Prerequisites
- Haptic Navigation Vest fully assembled
- Raspberry Pi 4 with real-time OS installed
- LiDAR sensor calibrated and working
- All motors tested and functional
- Battery fully charged

## Demo Setup (5 minutes)

1. **Power On**
   - Switch on the vest battery
   - LED indicator should show green (system ready)
   - Listen for motor test sequence (all motors vibrate briefly)

2. **Network Connection**
   - Connect to the Raspberry Pi via SSH: `ssh pi@<pi-ip-address>`
   - Navigate to project directory: `cd /root/haptic-nav-vest`

## Demo Sequence (10 minutes)

### Part 1: Static Environment Test (2 minutes)
- Place the vest in a known environment
- Run obstacle detection: `./run_demo.sh --static`
- Expected: Motors activate based on obstacles detected

### Part 2: Dynamic Navigation (5 minutes)
- User wears the vest
- Walk through environment with obstacles
- LiDAR continuously scans surroundings
- Motors provide haptic feedback for:
  - **Front obstacles**: Vibration in chest motor
  - **Side obstacles**: Vibration in corresponding sleeve
  - **Rear obstacles**: Vibration in back panel

### Part 3: Performance Metrics (3 minutes)
- System displays:
  - Detection latency: <100ms
  - Battery usage: Real-time power consumption
  - Motor response time: <50ms
  - Sensor accuracy: LiDAR detection range and reliability

## Expected Behaviors

✓ **Correct Operation:**
- Smooth wear, no discomfort
- Responsive haptic feedback (no delay)
- All motors activate appropriately

✗ **Failure Modes:**
- Check I2C connections if motor doesn't vibrate
- Restart Pi if sensor data freezes
- Charge battery if system shuts down unexpectedly

## Post-Demo Shutdown
1. Stop the demonstration: `Ctrl+C`
2. Shutdown Pi: `sudo shutdown -h now`
3. Disconnect battery
4. Inspect motors for any damage

## Success Criteria
- [x] All 6 motors respond to obstacles
- [x] Detection latency < 100ms
- [x] No system crashes during 10-minute operation
- [x] User provides positive feedback on haptic guidance
