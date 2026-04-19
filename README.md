# Haptic Navigation Vest — Real-Time Wearable Obstacle Detection

This GitHub repository contains a real-time embedded systems project for the ENG5220 course at the University of Glasgow. The Haptic Navigation Vest is designed to help visually impaired individuals navigate safely using LiDAR-based obstacle detection, IMU-based fall/stair detection, and haptic (vibration) + audio feedback.

## Table of Contents

- [Features](#features)
- [Building](#building)
- [Documentation](#Documentation)
- [Classes Within this GitHub](#classes-within-this-github)
- [User Case UML Diagram](#user-case-uml-diagram)
- [Circuit Diagram](#circuit-diagram)
- [Latency](#latency)
- [Acknowledgements](#acknowledgements)
- [Authors & Contributions](#authors-and-contributions)
- [License](#license)
- [Future Work](#future-work)
- [Instagram](#Instagram)
- [Hardware](#Hardware)
- [Report](#Report)

## Features

- **Obstacle Detection** — LiDAR sensor detects objects in the user's path (up to ~3 m range).
- **Fall Detection** — IMU detects sudden free-fall or high-g impact events.
- **Stair / Cliff Detection** — Pitch angle analysis from the accelerometer flags elevation changes.
- **Haptic Feedback** — 3 vibration motors mounted on the vest give directional warnings.
- **Audio Alerts** — Buzzer sounds for critical hazards (falls, very close obstacles).



### Cloning

```bash
git clone --recursive https://github.com/akshayabhavika153/ENG5220-Real-Time-Embedded-Programming_Haptic_Navigation_Vest.git
```

If submodules do not download:

```bash
git submodule update --init --recursive
```

### Libraries

```bash
sudo apt-get update
sudo apt-get install -y cmake g++ libgpiod-dev libi2c-dev
```

## Building

```bash
mkdir build && cd build
cmake ..
make
```

### Running Software Tests

```bash
cd build
make test
# or for verbose output:
ctest --verbose
```
## Documentation
Full project documentation can be found in the `/docs` folder:

- Project_overview
- Problem_statement
- Requirement
- System_Architecture
- Hardware_design
- Sesnor_selection
- Realtime_software_design
- Algorithm
- Testing_plan
- Risk_safety_ethics
- deomo_script
- progress_log
 

## Classes Within this GitHub

### /src/main

The main.cpp file acts as the central entry point of the system, responsible for initialising all modules and starting the program. It sets up components such as sensors, navigation logic, and event handlers, ensuring they are correctly connected. It then runs the main execution loop, allowing the system to operate continuously and respond to real-time events.

### /src/LidarSensor

I2C driver for the LiDAR distance sensor. Reads distance measurements in a background thread using blocking I/O and fires callbacks to subscribers when new data arrives.

### /src/IMUSensor

I2C driver for the MPU6050 IMU. Samples accelerometer and gyroscope data at ~100 Hz in its own thread and delivers readings through a callback interface.

### /src/HapticController

Controls 3 ERM vibration motors via GPIO. Accepts a hazard level and maps it to a vibration pattern (centre-only for distant obstacles, all motors for close/critical).

### /src/audioControler

Drives a piezo buzzer through a GPIO pin. Turned on for critical hazards and fall/stair events.

### /src/BatteryMonitor

The battery monitoring system tracks the device’s voltage level to ensure reliable operation. It continuously measures the battery and compares it against predefined thresholds to detect low or critical levels. When the battery is low, it triggers alerts or system actions to prevent sudden shutdown.

### /src/EmergencyController

The emergency controller manages critical situations such as fall detection by prioritising high-risk events over normal operation. It overrides other system functions to ensure immediate response when an emergency is detected. This ensures user safety by triggering alerts and maintaining control during critical conditions.

### /src/NavigationLogic

The Navigation Logic module processes sensor data to determine obstacle distance and direction. It uses classification and detection algorithms to decide the appropriate alert level or action. This enables the system to guide movement and respond safely to changes in the environment.

### /src/ProrityManager

The Priority Manager handles incoming events by assigning and comparing their priority levels. It ensures that critical events, such as emergencies, override normal system operations. This allows the system to respond quickly and appropriately to the most important situations.

### /src/SensorHealth 

The Sensor Health module monitors the status and reliability of sensor data. It validates readings to ensure they are within acceptable ranges and detects faults or errors. This helps maintain system accuracy by preventing invalid or noisy data from affecting decisions.

## Include
The include.txt file lists all required header files and dependencies used in the project. It helps organise and document which components are needed for successful compilation. This ensures consistency and simplifies project setup.

### IMUSensor.hpp
The IMUSensor.hpp file defines the interface for interacting with the IMU (Inertial Measurement Unit) sensor. It provides methods to read motion and orientation data such as acceleration and angular movement. This allows the system to detect motion-related events and support features like fall detection.

### NavigationLogic.hpp
The NavigationLogic.hpp file declares the functions and structures used for processing sensor data and determining navigation decisions. It defines how obstacle distances and directions are interpreted into alert levels or actions. This serves as the core interface for the navigation system.

### PriorityManager.hpp
The PriorityManager.hpp file defines the interface for managing event priorities within the system. It includes declarations for handling events and ensuring that critical situations override normal operations. This helps maintain proper control flow in real-time conditions.


## User Case UML Diagram

The system boots and initialises the LiDAR, IMU, motors, and sound sensor. The LiDAR thread continuously reads distance data while the IMU thread reads motion data. The decision engine fuses these inputs, classifies the hazard, and immediately activates the appropriate motors and buzzer. This loop runs continuously in real time.

<img width="422" height="591" alt="image" src="https://github.com/user-attachments/assets/e83d11a8-f06c-4686-8f68-3b39aee1df9d" />


## Circuit Diagram

The LiDAR communicates over I2C, the IMU shares the same I2C bus at a different address, and the three vibration motors are driven via GPIO pins through a motor driver IC. The buzzer is driven directly from a GPIO pin. The Raspberry Pi serves as the central controller.
<img width="992" height="780" alt="image" src="https://github.com/user-attachments/assets/4051f633-49ef-4d8f-8730-274825d0f278" />



## Latency

### LiDAR Sensor

The LiDAR operates at ~10 Hz. Each blocking I2C read completes quickly, and the callback fires immediately after. Overall sensing latency is well under 100 ms per sample.

### IMU Sensor

The IMU runs at ~100 Hz. The I2C burst read of 14 bytes takes a fraction of a millisecond, keeping detection of falls and orientation changes responsive.

### End-to-End

The decision engine runs in the sensor callback context, so the delay from sensing to motor actuation is minimal. The full pipeline target is under 100 ms.

## Acknowledgements

We would like to thank **Dr. Bernd Porr** for supervising this project as part of the ENG5220 Real-Time Embedded Programming course at the University of Glasgow.

## Authors

- Akshaya Bhavika Banda
- Mandasmitha
- Bibid
- Harris
- Abhinav

## License

This project is licensed under the [GPL-3.0 License](LICENSE).

## Future Work

- Refine sensor fusion to reduce false-positive fall detections.
- Add PWM-based vibration intensity control instead of simple on/off.
- Design a custom PCB to replace the breadboard wiring.
- Investigate adding a second LiDAR sensor for wider field of view.

## Instagram
- ID: https://www.instagram.com/haptic_navigation_vest?igsh=eHllaTFvbmg0Ym52

## Hardware
- 3D print Design
  <img width="828" height="756" alt="image" src="https://github.com/user-attachments/assets/aba070f3-3b38-480b-b59f-223845835edd" />
  <img width="542" height="673" alt="image" src="https://github.com/user-attachments/assets/bb839c46-a518-453b-839f-be27412cf5ca" />
  <img width="634" height="632" alt="image" src="https://github.com/user-attachments/assets/e813a9bb-acf4-490c-8ee5-431d7d8d413c" />
  <img width="600" height="561" alt="image" src="https://github.com/user-attachments/assets/8331bc33-bdc9-4d1f-9dd5-e9684b8116a5" />
  <img width="1313" height="1600" alt="image" src="https://github.com/user-attachments/assets/ce754cb7-be61-4f32-b373-d64355b8d3a3" />
  <img width="990" height="1024" alt="image" src="https://github.com/user-attachments/assets/d5be20fa-5a10-4ff0-919d-4e07695f27f2" />



## Report
https://gla-my.sharepoint.com/:b:/g/personal/3129723a_student_gla_ac_uk/IQBv5vHlsm6dQ6tLBdSN29omAUG6LghDywNdJCTIIJoPhVc?e=TMYyrw




