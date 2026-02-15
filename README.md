# Haptic Navigation Vest
## Hardware Architecture

The Haptic Navigation Vest relies on a suite of sensors and actuators to provide real-time feedback:

### Perception & Safety
* **Lidar (Environmental Perception):** Scans the immediate surroundings to detect static and dynamic obstacles.
* **IMU (Accelerometer & Gyroscope):** Monitors user kinematics to detect sudden falls or collisions, triggering emergency alerts if necessary.

### Feedback Systems
* **Haptic Feedback (Vibration Motor):** Delivers tactile navigation cues. Different vibration patterns indicate the proximity and direction of obstacles.
* **Audio Alerts (Speaker):** Provides supplementary sound warnings for critical hazards or system status updates.

### Drivers
* **Motor Driver:** Custom C++ driver to manage vibration intensity via PWM, ensuring low-latency response to Lidar data.
