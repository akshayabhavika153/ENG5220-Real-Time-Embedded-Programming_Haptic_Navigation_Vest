# Project Overview

## Haptic Navigation Vest — Real-Time Wearable Obstacle Detection

### What is it?

The Haptic Navigation Vest is a **real-time embedded wearable system** that helps visually impaired individuals navigate safely by detecting obstacles, falls, and elevation changes, and communicating hazards through vibration and audio feedback.

### Why does it matter?

Over 2 billion people globally have vision impairment. Existing aids (white canes, guide dogs) have significant limitations in detecting obstacles at different heights and providing real-time, directional hazard warnings. This project provides a **low-cost, hands-free, intuitive alternative**.

### How does it work?

| Step | Action | Component |
|------|--------|-----------|
| 1    | **Sense** — Scan the environment for obstacles and motion | LiDAR sensor + IMU |
| 2    | **Process** — Classify hazards in real-time | Raspberry Pi (C++ software) |
| 3    | **Act** — Alert the user via vibration patterns and audio | 3 vibration motors + buzzer |

### Key Specifications

| Specification             | Value                                   |
|---------------------------|-----------------------------------------|
| Platform                  | Raspberry Pi 4/5                        |
| Language                  | C++17                                   |
| Build System              | CMake                                   |
| Sensors                   | LiDAR (obstacle), IMU (fall/tilt)       |
| Actuators                 | 3 vibration motors, 1 buzzer/speaker    |
| Target latency            | <100 ms end-to-end                      |
| Power                     | Portable battery (≥2 hr runtime target) |
| Weight                    | <1.5 kg total                           |

### Team

| Member          | Role                                  |
|-----------------|---------------------------------------|
| Akshaya Bhavika | LiDAR driver, main integration        |
| Mandasmitha     | IMU driver, hardware design           |
| Bibid           | Decision engine, algorithms           |
| Harris          | Haptic/audio controllers, CMake build |
| Abhinav         | Documentation, testing, project mgmt  |

### Course

**ENG5220 — Real-Time Embedded Programming**  
University of Glasgow  
Academic Year 2025–2026

### Repository Structure

```
├── README.md                    # Main project README
├── CMakeLists.txt               # Build system configuration
├── src/                         # Source code
│   ├── lidar/                   # LiDAR sensor driver
│   ├── imu/                     # IMU sensor driver
│   ├── haptic/                  # Vibration motor controller
│   ├── audio/                   # Audio alert controller
│   ├── fusion/                  # Decision engine
│   └── main.cpp                 # Entry point
├── test/                        # Unit tests
├── docs/                        # Technical documentation
├── Images/                      # Diagrams, photos, circuit schematics
├── Datasheet/                   # Component datasheets
└── LICENSE                      # License file
```
