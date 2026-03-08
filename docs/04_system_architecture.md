# System Architecture

## 1. Overview

The Haptic Navigation Vest is a multi-sensor, real-time embedded system built on a Raspberry Pi. The architecture follows a **sense → process → act** pipeline designed for low-latency operation.

```
┌─────────────────────────────────────────────────────────────────┐
│                      HAPTIC NAVIGATION VEST                     │
│                                                                 │
│  ┌───────────────────────┐    ┌──────────────────────────────┐  │
│  │     SENSOR LAYER      │    │      ACTUATOR LAYER          │  │
│  │                       │    │                              │  │
│  │  ┌────────┐ ┌───────┐ │    │  ┌──────────┐ ┌───────────┐ │  │
│  │  │ LiDAR  │ │  IMU  │ │    │  │ Vibration│ │  Buzzer / │ │  │
│  │  │ Sensor │ │MPU6050│ │    │  │ Motors x3│ │  Speaker  │ │  │
│  │  └───┬────┘ └───┬───┘ │    │  └────┬─────┘ └─────┬─────┘ │  │
│  └──────┼──────────┼─────┘    └───────┼─────────────┼───────┘  │
│         │          │                  │             │           │
│         │ I2C/UART │ I2C              │ GPIO        │ PWM/GPIO │
│         │          │                  │             │           │
│  ┌──────┼──────────┼──────────────────┼─────────────┼───────┐  │
│  │      ▼          ▼                  ▲             ▲       │  │
│  │  ┌──────────────────────────────────────────────────────┐│  │
│  │  │              RASPBERRY PI (Processing Unit)          ││  │
│  │  │                                                      ││  │
│  │  │  ┌──────────┐  ┌───────────┐  ┌──────────────────┐  ││  │
│  │  │  │  Sensor   │  │  Decision  │  │   Motor / Audio  │  ││  │
│  │  │  │  Drivers  │─▶│  Engine    │─▶│   Controller     │  ││  │
│  │  │  │ (LiDAR,   │  │ (Fusion,  │  │ (Haptic + Buzz)  │  ││  │
│  │  │  │  IMU)     │  │  Classify) │  │                  │  ││  │
│  │  │  └──────────┘  └───────────┘  └──────────────────┘  ││  │
│  │  └──────────────────────────────────────────────────────┘│  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │                    POWER SUBSYSTEM                       │    │
│  │   Battery Pack → 5V Regulator → Pi + Sensors + Motors   │    │
│  └─────────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────────┘
```

---

## 2. Hardware Architecture

### 2.1 Component Interconnection

| Component              | Interface      | Connected To              | Protocol / Notes               |
|------------------------|----------------|---------------------------|--------------------------------|
| LiDAR (TFmini/VL53L1X) | I2C or UART   | Raspberry Pi GPIO         | Distance measurements          |
| IMU (MPU6050/ICM-20948) | I2C           | Raspberry Pi (SDA, SCL)   | 6-axis accel + gyro data       |
| Vibration Motor 1      | GPIO           | Motor Driver → Pi GPIO 17 | Left-side haptic feedback      |
| Vibration Motor 2      | GPIO           | Motor Driver → Pi GPIO 27 | Centre haptic feedback         |
| Vibration Motor 3      | GPIO           | Motor Driver → Pi GPIO 22 | Right-side haptic feedback     |
| Buzzer / Speaker       | PWM / GPIO     | Pi GPIO 18                | Audio alert for critical events|
| Motor Driver (L293D)   | —              | Between Pi GPIO and motors | Current amplification          |
| Power Supply           | —              | All components             | 5V regulated                   |

### 2.2 Circuit Diagram

<!-- TODO: Replace with your actual circuit diagram image -->
<!-- ![Circuit Diagram](../Images/circuit_diagram.png) -->

*Circuit diagram to be added once hardware wiring is finalized.*

### 2.3 Physical Layout on Vest

<!-- TODO: Add a diagram or photo showing motor placement on the vest -->

```
        ┌─────────────────────────┐
        │       VEST (Front)       │
        │                         │
        │  [M1-Left]   [M3-Right] │  ← Vibration motors at chest level
        │         [M2-Centre]     │
        │                         │
        │     ┌──────────────┐    │
        │     │ Raspberry Pi │    │  ← Mounted in back pocket/pouch
        │     │ + Battery    │    │
        │     └──────────────┘    │
        │                         │
        │  [LiDAR]  (front-facing)│  ← Mounted at chest height
        │  [IMU]    (chest/waist) │
        │  [Buzzer] (shoulder)    │
        └─────────────────────────┘
```

---

## 3. Software Architecture

### 3.1 Module Overview

```
src/
├── lidar/              # LiDAR sensor driver
│   ├── lidar_driver.h
│   ├── lidar_driver.cpp
│   └── CMakeLists.txt
├── imu/                # IMU sensor driver
│   ├── imu_driver.h
│   ├── imu_driver.cpp
│   └── CMakeLists.txt
├── haptic/             # Vibration motor controller
│   ├── haptic_controller.h
│   ├── haptic_controller.cpp
│   └── CMakeLists.txt
├── audio/              # Buzzer / audio alert controller
│   ├── audio_controller.h
│   ├── audio_controller.cpp
│   └── CMakeLists.txt
├── fusion/             # Sensor fusion & decision algorithm
│   ├── decision_engine.h
│   ├── decision_engine.cpp
│   └── CMakeLists.txt
├── main.cpp            # Application entry point
└── CMakeLists.txt      # Top-level CMake
test/
├── test_lidar.cpp
├── test_imu.cpp
├── test_haptic.cpp
├── test_fusion.cpp
└── CMakeLists.txt
```

### 3.2 Data Flow

```
LiDAR Sensor ──(distance_cm)──▶ ┌──────────────────┐
                                 │  Decision Engine  │──▶ Haptic Controller ──▶ Vibration Motors
IMU Sensor ──(accel, gyro)────▶  │  (Sensor Fusion)  │──▶ Audio Controller ──▶ Buzzer
                                 └──────────────────┘
```

### 3.3 Main Loop (Pseudocode)

```cpp
int main() {
    // Initialize hardware
    LidarDriver lidar("/dev/i2c-1", LIDAR_ADDR);
    IMUDriver imu("/dev/i2c-1", IMU_ADDR);
    HapticController haptic({GPIO_17, GPIO_27, GPIO_22});
    AudioController audio(GPIO_18);

    // Real-time loop
    while (running) {
        // 1. SENSE
        float distance = lidar.readDistance();
        IMUData imu_data = imu.readData();

        // 2. PROCESS
        HazardType hazard = DecisionEngine::classify(distance, imu_data);

        // 3. ACT
        haptic.activate(hazard);
        audio.alert(hazard);

        // 4. Maintain loop timing
        sleep_until(next_cycle);  // e.g., 20 ms for 50 Hz
    }
    return 0;
}
```

### 3.4 Class Diagram (Planned)

<!-- TODO: Replace with actual UML class diagram image -->
<!-- ![Class Diagram](../Images/class_diagram.png) -->

```
┌───────────────┐      ┌──────────────┐      ┌───────────────────┐
│  LidarDriver  │      │  IMUDriver   │      │  DecisionEngine   │
├───────────────┤      ├──────────────┤      ├───────────────────┤
│ -i2c_fd: int  │      │ -i2c_fd: int │      │ -obstacle_thresh  │
│ -address: int │      │ -address: int│      │ -fall_thresh      │
├───────────────┤      ├──────────────┤      ├───────────────────┤
│ +init()       │      │ +init()      │      │ +classify()       │
│ +readDist()   │      │ +readAccel() │      │ +fuseData()       │
│ +close()      │      │ +readGyro()  │      └───────┬───────────┘
└───────┬───────┘      │ +close()     │              │
        │              └──────┬───────┘              │
        │                     │                      │
        └─────────┬───────────┘              ┌───────▼───────────┐
                  │                          │ HapticController  │
                  │                          ├───────────────────┤
                  │                          │ -gpio_pins[]      │
                  ▼                          ├───────────────────┤
          ┌───────────────┐                  │ +activate()       │
          │ AudioController│                 │ +setIntensity()   │
          ├───────────────┤                  │ +stop()           │
          │ -gpio_pin     │                  └───────────────────┘
          ├───────────────┤
          │ +alert()      │
          │ +stop()       │
          └───────────────┘
```

---

## 4. Communication Protocols

| Protocol | Used For                    | Details                                         |
|----------|-----------------------------|-------------------------------------------------|
| I2C      | IMU data, LiDAR (if I2C)   | SDA = GPIO 2, SCL = GPIO 3, 400 kHz fast mode   |
| UART     | LiDAR (if UART model)      | GPIO 14 (TX), GPIO 15 (RX), 115200 baud         |
| GPIO     | Motor driver control        | Digital HIGH/LOW or PWM for intensity control    |
| PWM      | Buzzer tone generation      | Hardware PWM on GPIO 18                          |

---

## 5. Power Architecture

<!-- TODO: Update with your actual power measurements -->

| Component          | Voltage | Estimated Current | Notes                     |
|--------------------|---------|-------------------|---------------------------|
| Raspberry Pi       | 5V      | ~600–1200 mA      | Depends on processing load|
| LiDAR Sensor       | 3.3/5V  | ~100–150 mA       | During active scanning    |
| IMU                | 3.3V    | ~5 mA             | Very low power            |
| Vibration Motor x3 | 3–5V    | ~80 mA each       | When active               |
| Buzzer             | 3.3/5V  | ~30 mA            | When sounding             |
| **Total (peak)**   |         | **~1.5–2.0 A**    | Size battery accordingly  |

---

## 6. Threading / Concurrency Model

<!-- TODO: Choose and document your threading approach -->

**Option A — Single-threaded polling loop** (simpler):
- One main loop polls all sensors sequentially
- Suitable if total cycle time < target period

**Option B — Multi-threaded** (better performance):
- Thread 1: LiDAR reading (blocking I2C read)
- Thread 2: IMU reading (high-frequency polling)
- Thread 3: Decision engine + actuator control
- Use mutex-protected shared data structures

```
Thread 1 (LiDAR) ──▶ shared_distance ──┐
                                        ├──▶ Thread 3 (Decision + Actuation)
Thread 2 (IMU) ────▶ shared_imu_data ──┘
```

---

## 7. Deployment

The system runs as a standalone C++ application on Raspberry Pi OS. No external server, cloud, or smartphone connection is required. The application starts automatically on boot (via systemd service or cron) and runs continuously until powered off.
