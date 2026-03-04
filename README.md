# Haptic Navigation Vest — Real-Time Wearable Obstacle Detection

This GitHub repository contains a real-time embedded systems project for the ENG5220 course at the University of Glasgow. The Haptic Navigation Vest is designed to help visually impaired individuals navigate safely using LiDAR-based obstacle detection, IMU-based fall/stair detection, and haptic (vibration) + audio feedback.

## Table of Contents

- [Features](#features)
- [Bill of Materials](#bill-of-materials)
- [Building](#building)
- [Classes Within this GitHub](#classes-within-this-github)
- [User Case UML Diagram](#user-case-uml-diagram)
- [Circuit Diagram](#circuit-diagram)
- [Latency](#latency)
- [Acknowledgements](#acknowledgements)
- [Authors & Contributions](#authors-and-contributions)
- [License](#license)
- [Future Work](#future-work)

## Features

- **Obstacle Detection** — LiDAR sensor detects objects in the user's path (up to ~3 m range).
- **Fall Detection** — IMU detects sudden free-fall or high-g impact events.
- **Stair / Cliff Detection** — Pitch angle analysis from the accelerometer flags elevation changes.
- **Haptic Feedback** — 3 vibration motors mounted on the vest give directional warnings.
- **Audio Alerts** — Buzzer sounds for critical hazards (falls, very close obstacles).

## Bill of Materials

### Controller

| Microcontroller          | Quantity | Cost (£) |
|--------------------------|----------|----------|
| Raspberry Pi 4 Model B   | 1        | —        |

### Sensors

| Sensor                   | Quantity | Cost (£) |
|--------------------------|----------|----------|
| LiDAR (TFmini-S)        | 1        | 18.46    |
| IMU (MPU6050)            | 1        | 11.91    |

### Supporting Components

| Component                | Quantity | Cost (£) |
|--------------------------|----------|----------|
| Vibration Motor (ERM)    | 3        | 3.33     |
| Piezo Buzzer             | 1        | 18.70    |
| Motor Driver (L293D)     | 1        | —        |
| Battery Pack             | 1        | —        |
| Vest / Garment           | 1        | —        |
| Wiring, Connectors       | —        | —        |

**Grand Total:** ~£52.40

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

## Classes Within this GitHub

### /src/lidar

I2C driver for the LiDAR distance sensor. Reads distance measurements in a background thread using blocking I/O and fires callbacks to subscribers when new data arrives.

### /src/imu

I2C driver for the MPU6050 IMU. Samples accelerometer and gyroscope data at ~100 Hz in its own thread and delivers readings through a callback interface.

### /src/haptic

Controls 3 ERM vibration motors via GPIO. Accepts a hazard level and maps it to a vibration pattern (centre-only for distant obstacles, all motors for close/critical).

### /src/audio

Drives a piezo buzzer through a GPIO pin. Turned on for critical hazards and fall/stair events.

### /src/fusion

The decision engine subscribes to both LiDAR and IMU callbacks. It classifies obstacle distance into hazard levels, detects falls from accelerometer magnitude, and detects stairs from pitch angle. Then it drives the haptic and audio outputs accordingly.

## User Case UML Diagram

The system boots and initialises the LiDAR, IMU, motors, and buzzer. The LiDAR thread continuously reads distance data while the IMU thread reads motion data. The decision engine fuses these inputs, classifies the hazard, and immediately activates the appropriate motors and buzzer. This loop runs continuously in real time.

*(Diagram to be added)*

## Circuit Diagram

The LiDAR communicates over I2C, the IMU shares the same I2C bus at a different address, and the three vibration motors are driven via GPIO pins through a motor driver IC. The buzzer is driven directly from a GPIO pin. The Raspberry Pi serves as the central controller.

*(Diagram to be added)*

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

- Akshaya Bhavika
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



## Last Updated

This README was last updated on 04/03/2026.
