# Requirements Specification

## 1. Introduction

This document defines the functional and non-functional requirements for the Haptic Navigation Vest. Requirements are categorized using the MoSCoW prioritization method (Must, Should, Could, Won't).

---

## 2. Functional Requirements

### 2.1 Sensing

| ID     | Requirement                                                                     | Priority | Status      |
|--------|---------------------------------------------------------------------------------|----------|-------------|
| FR-S01 | The system **must** read distance measurements from the LiDAR sensor in real-time | Must     | <!-- TODO --> |
| FR-S02 | The system **must** read acceleration and gyroscope data from the IMU            | Must     | <!-- TODO --> |
| FR-S03 | The LiDAR **must** detect obstacles within a range of 0.3 m to 4.0 m            | Must     | <!-- TODO --> |
| FR-S04 | The IMU **should** sample at ≥100 Hz for accurate fall detection                | Should   | <!-- TODO --> |
| FR-S05 | The system **could** support a secondary downward-facing sensor for cliff detection | Could | <!-- TODO --> |

### 2.2 Processing & Decision-Making

| ID     | Requirement                                                                     | Priority | Status      |
|--------|---------------------------------------------------------------------------------|----------|-------------|
| FR-P01 | The system **must** classify obstacles by proximity (far / medium / close / critical) | Must | <!-- TODO --> |
| FR-P02 | The system **must** detect a fall event based on IMU acceleration thresholds     | Must     | <!-- TODO --> |
| FR-P03 | The system **should** detect stair/cliff conditions using IMU pitch angle        | Should   | <!-- TODO --> |
| FR-P04 | The system **should** fuse LiDAR and IMU data for robust decision-making        | Should   | <!-- TODO --> |
| FR-P05 | The main processing loop **must** complete each cycle within 100 ms             | Must     | <!-- TODO --> |

### 2.3 Feedback / Actuation

| ID     | Requirement                                                                     | Priority | Status      |
|--------|---------------------------------------------------------------------------------|----------|-------------|
| FR-F01 | The system **must** activate vibration motors to indicate obstacle proximity    | Must     | <!-- TODO --> |
| FR-F02 | Vibration intensity **should** increase as obstacle distance decreases          | Should   | <!-- TODO --> |
| FR-F03 | The system **must** trigger an audio alert (buzzer) for critical hazards        | Must     | <!-- TODO --> |
| FR-F04 | The system **could** provide different vibration patterns for different hazard types | Could | <!-- TODO --> |
| FR-F05 | The system **must** use 3 vibration motors for directional feedback (left, centre, right) | Must | <!-- TODO --> |

### 2.4 System Control

| ID     | Requirement                                                                     | Priority | Status      |
|--------|---------------------------------------------------------------------------------|----------|-------------|
| FR-C01 | The system **must** initialize all sensors and actuators on startup             | Must     | <!-- TODO --> |
| FR-C02 | The system **must** operate as a continuous real-time loop                      | Must     | <!-- TODO --> |
| FR-C03 | The system **should** gracefully shut down on user command or signal            | Should   | <!-- TODO --> |
| FR-C04 | The system **could** log sensor data to a file for post-analysis               | Could    | <!-- TODO --> |

---

## 3. Non-Functional Requirements

| ID      | Requirement                                                                    | Priority | Status      |
|---------|--------------------------------------------------------------------------------|----------|-------------|
| NFR-01  | End-to-end latency (sensor read → motor activation) **must** be <100 ms        | Must     | <!-- TODO --> |
| NFR-02  | The system **must** run on a Raspberry Pi (Model 4 or 5)                       | Must     | <!-- TODO --> |
| NFR-03  | The software **must** be written in C++ for performance                        | Must     | <!-- TODO --> |
| NFR-04  | The project **must** use CMake as the build system                             | Must     | <!-- TODO --> |
| NFR-05  | The system **should** operate on portable battery power for ≥2 hours           | Should   | <!-- TODO --> |
| NFR-06  | The vest **should** weigh less than 1.5 kg (including all components)          | Should   | <!-- TODO --> |
| NFR-07  | The system **must** have unit tests integrated with CMake (`ctest`)            | Must     | <!-- TODO --> |
| NFR-08  | All public classes/functions **must** be documented using Doxygen comments     | Must     | <!-- TODO --> |
| NFR-09  | The system **should** be reproducible from the GitHub repository instructions  | Should   | <!-- TODO --> |
| NFR-10  | The software **should** handle sensor failures gracefully (e.g., I2C timeout)  | Should   | <!-- TODO --> |

---

## 4. Hardware Constraints

| Constraint                                         | Detail                                      |
|----------------------------------------------------|---------------------------------------------|
| Processor                                          | Raspberry Pi 4/5 (ARM Cortex-A72/A76)       |
| Sensor interfaces                                  | I2C (IMU), UART or I2C (LiDAR), GPIO        |
| Actuator interfaces                                | GPIO (motors via driver), PWM (buzzer)       |
| Power                                              | 5V USB-C (Pi) + motor/buzzer power supply    |
| Physical form factor                               | Wearable vest, components securely mounted   |

---

## 5. Acceptance Criteria

| Test                                   | Pass Condition                                              |
|----------------------------------------|-------------------------------------------------------------|
| Obstacle detection accuracy            | ≥90% of obstacles in 0.3–4 m detected correctly            |
| Fall detection                         | Fall event detected within 500 ms of occurrence             |
| Haptic feedback perceptibility         | User correctly identifies obstacle direction ≥80% of time   |
| System latency                         | End-to-end <100 ms measured via timestamp logging            |
| Battery life                           | ≥2 hours continuous operation                                |
| Build reproducibility                  | Clone → cmake → make → run succeeds on clean Raspberry Pi   |

---

## 6. Traceability Matrix

<!-- TODO: Map each requirement to its implementing module and test case -->

| Requirement | Module           | Test Case |
|-------------|------------------|-----------|
| FR-S01      | `src/lidar/`     | TBD       |
| FR-S02      | `src/imu/`       | TBD       |
| FR-F01      | `src/haptic/`    | TBD       |
| FR-F03      | `src/audio/`     | TBD       |
| FR-P01      | `src/fusion/`    | TBD       |
