# Hardware Design

## 1. Introduction

This document describes the hardware architecture of the Haptic Navigation Vest.  
The system integrates sensing components, a processing unit, and feedback devices to provide obstacle detection and fall detection support for the user.

The design focuses on:

- Safe electrical integration  
- Modular structure  
- Wearable implementation  
- Reliable power supply  
- Expandability for future improvements  

---

## 2. Hardware Components Used

The system consists of the following hardware components:

### Main Controller
- Raspberry Pi  

### Sensors
- LiDAR (Obstacle detection)  
- Accelerometer + Gyroscope (IMU)  
- Vibration sensor (impact detection)  

### Actuators
- Vibration motors  
- Buzzer  
- Speaker  

### Supporting Components
- Driver circuit  
- Power module  
- Breadboard and jumper wires (lab provided)  

---

## 3. System Overview

The hardware operates using the following flow:

1. Sensors collect environmental and motion data.
2. Raspberry Pi processes the data.
3. Driver circuit controls vibration motors.
4. Buzzer and speaker provide audio alerts.
5. Power module supplies regulated power to all components.

---

## 4. Sensor Description

### 4.1 LiDAR Sensor

**Purpose:**  
Detect obstacles in front of the user.

**Function:**  
- Measures distance to objects.
- Helps determine proximity level.
- Triggers vibration feedback when obstacles are near.

---

### 4.2 Accelerometer + Gyroscope (IMU)

**Purpose:**  
Detect body motion and fall events.

**Function:**  
- Measures acceleration.
- Detects sudden motion changes.
- Helps identify fall or tilt conditions.

---

### 4.3 Vibration Sensor

**Purpose:**  
Detect sudden physical impact.

**Function:**  
- Detects shock or strong vibration.
- Supports fall detection mechanism.
- Adds safety redundancy.

---

## 5. Actuator Description

### 5.1 Vibration Motors

**Purpose:**  
Provide haptic feedback to the user.

**Operation:**  
- Motors activate based on obstacle distance.
- Intensity increases as object gets closer.
- Positioned to provide directional feedback.

---

### 5.2 Buzzer

**Purpose:**  
Provide simple alert signals.

Used for:
- Fall detection warning
- Immediate hazard alert

---

### 5.3 Speaker

**Purpose:**  
Provide clearer audio alerts.

Used for:
- Stair warning
- Fall notification
- System alerts

---

## 6. Driver Circuit Design

Raspberry Pi GPIO pins cannot directly power vibration motors because:

- GPIO operates at low voltage.
- Current supplied is limited.
- Motors require higher current.

To safely control the motors, we use:

- NPN transistor or MOSFET as a switching driver
- Flyback diode for motor protection
- External 5V battery supply for motors
- Common ground between Raspberry Pi and motor power supply

This protects the Raspberry Pi and ensures stable motor operation.

---

## 7. Power Module

The system is powered using a rechargeable battery and voltage regulation module.

### Requirements:
- Stable 5V supply
- Sufficient current for all components
- Proper grounding

The power module ensures safe and consistent operation of the wearable system.

---

## 8. Prototyping Setup

Breadboard and jumper wires (provided by lab) are used for:

- Initial circuit testing
- Sensor connection validation
- Driver circuit testing
- Output device testing

This allows flexible modifications during development.

---

## 9. Safety Considerations

- Motors are not directly connected to GPIO.
- Driver circuit isolates high-current components.
- Flyback diode prevents voltage spikes.
- Stable power supply prevents voltage fluctuation.
- All wiring must be securely connected to avoid short circuits.

---

## 10. Summary

The hardware system integrates:

- Obstacle sensing (LiDAR)
- Motion sensing (IMU)
- Impact detection (Vibration sensor)
- Haptic feedback (Vibration motors)
- Audio alerts (Buzzer and Speaker)
- Safe motor control (Driver circuit)
- Regulated power supply (Power module)

This hardware architecture provides a safe and modular foundation for implementing the Haptic Navigation Vest.