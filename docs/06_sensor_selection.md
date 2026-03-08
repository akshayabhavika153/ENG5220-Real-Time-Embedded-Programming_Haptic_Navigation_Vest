# Main Controller
- Raspberry Pi
Central processing unit
Runs Linux (Raspberry Pi OS)
Handles sensor reading, decision logic, and output control

# Sensors
1. LiDAR
Used for obstacle detection
Measures distance to objects in front of the user
2. IMU (Accelerometer + Gyroscope)
Used for fall detection
Monitors motion and body orientation
3. Vibration Sensor
Detects sudden impact or shock
Supports fall confirmation

# Actuators
1. Vibration Motors
Provide directional haptic feedback
Intensity increases as obstacle distance decreases
2. Buzzer / Speaker
Provide audio alerts
Used for fall warnings and critical hazard alerts

# Driver Circuit
GPIO pins cannot directly power vibration motors because:
GPIO operates at 3.3V logic level
Current supplied by GPIO is limited

# To safely control motors, we use:
- NPN transistor or MOSFET as switching driver
- Flyback diode for motor protection
- External 5V battery supply for motors
- Common ground between Raspberry Pi and motor supply
This prevents damage to the Raspberry Pi and ensures stable operation.

# Power Module
1. Rechargeable battery
2. Voltage regulation to provide stable 5V supply
3. Common ground reference for all components