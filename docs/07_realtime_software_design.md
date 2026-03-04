# Real-Time Software Design

## 1. Introduction

This document describes how the Haptic Navigation Vest software is structured to meet real-time constraints. The system reads sensor data, classifies hazards, and drives actuators within a target end-to-end latency of **< 100 ms**.

---

## 2. Real-Time Requirements

| Parameter                    | Target Value    | Justification                                          |
|------------------------------|-----------------|--------------------------------------------------------|
| End-to-end latency           | < 100 ms        | Warning before the wearer reaches the hazard           |
| LiDAR sampling rate          | ~10 Hz          | Adequate for walking-speed obstacle detection          |
| IMU sampling rate            | >= 100 Hz       | Needed for accurate fall detection                     |
| Motor activation latency     | < 5 ms          | GPIO write is near-instantaneous                       |
| Sensor I2C read time         | < 10 ms         | I2C at 400 kHz fast mode                               |

---

## 3. Architecture

### 3.1 Event-Driven with Blocking I/O

The system is event-driven rather than poll-based. Each sensor has its own thread that performs blocking I2C reads. When data arrives, a callback fires immediately and the decision engine classifies the hazard and actuates the motors — all within the same thread context. This avoids unnecessary context switches and keeps latency predictable.

```cpp
// Optional real-time scheduler setup (run as root on the Pi)
#include <sched.h>
#include <sys/mman.h>

void setupRealTime() {
    mlockall(MCL_CURRENT | MCL_FUTURE);
    struct sched_param param;
    param.sched_priority = 49;
    sched_setscheduler(0, SCHED_FIFO, &param);
}
```

### 3.2 Publisher–Subscriber Model

Each sensor is a **publisher** with a callback interface. The `DecisionEngine` is the **subscriber** — it implements both `LidarCallback` and `IMUCallback` and gets called directly from the sensor threads.

```cpp
// Simplified view of the data flow:

class LidarDriver {
    struct LidarCallback {
        virtual void hasDistance(float distance_cm) = 0;
    };
    void registerCallback(LidarCallback* cb);
    void start();  // launches a thread with blocking I2C reads
};

class DecisionEngine : public LidarDriver::LidarCallback,
                        public IMUDriver::IMUCallback {
    void hasDistance(float cm) override {
        HazardLevel level = classifyObstacle(cm);
        haptic_.activate(level);
    }
    void hasIMUData(const IMUData& d) override {
        if (detectFall(d.accel_x, d.accel_y, d.accel_z))
            audio_.alert();
    }
};

int main() {
    // create modules, wire callbacks, start threads, sleep
    LidarDriver lidar("/dev/i2c-1", 0x10);
    DecisionEngine engine(haptic, audio);
    lidar.registerCallback(&engine);
    lidar.start();
    while (running) sleep(1);  // main does nothing
}
```

### 3.3 Timing Budget

```
LiDAR thread: [blocking read ~5 ms] -> [classify + actuate ~1 ms] -> [back to blocking]
IMU thread:   [blocking read ~2 ms] -> [fall detect ~0.5 ms] -> [back to blocking]
main thread:  [sleeping — 0% CPU]

End-to-end: sensor -> callback -> motor activation < 10 ms
```

---

## 4. Threading Design

### 4.1 Thread Map

```
Thread 1: LiDAR driver
  - Blocks on I2C read() until distance data is ready
  - Fires callback -> engine classifies obstacle and sets motors

Thread 2: IMU driver
  - Blocks on I2C read() at ~100 Hz
  - Fires callback -> engine checks for fall/stair events

Main thread:
  - Creates all objects, registers callbacks, starts threads
  - Sleeps until Ctrl-C
```

### 4.2 Thread Safety

Callbacks from different sensor threads may access shared state in `DecisionEngine`. This is protected with `std::mutex`:

```cpp
std::mutex mtx_;
std::atomic<HazardLevel> current_level_{HazardLevel::NONE};
std::atomic<bool> fall_detected_{false};

void hasDistance(float cm) override {
    HazardLevel level = classifyObstacle(cm);
    {
        std::lock_guard<std::mutex> lock(mtx_);
        current_level_ = level;
    }
    haptic_.activate(level);
}
```

---

## 5. Sensor Drivers

### 5.1 LiDAR Driver (`src/lidar/`)

Reads distance via I2C in a background thread. The blocking `read()` call naturally paces the loop at the sensor's output data rate.

### 5.2 IMU Driver (`src/imu/`)

Reads 14 bytes (accel + gyro) from the MPU6050 at ~100 Hz. Uses `sleep_until` to maintain consistent timing between I2C burst reads.

---

## 6. Decision Algorithm

### 6.1 Obstacle Classification

| Distance (cm) | HazardLevel |
|----------------|-------------|
| > 300          | NONE        |
| 200 – 300      | FAR         |
| 100 – 200      | MEDIUM      |
| 30 – 100       | CLOSE       |
| < 30           | CRITICAL    |

### 6.2 Fall Detection

Computes accelerometer magnitude: `sqrt(ax² + ay² + az²)`.
- < 0.3 g → freefall
- \> 2.5 g → impact

### 6.3 Stair / Cliff Detection

Computes pitch angle from `atan2(ax, az)`. If the absolute pitch exceeds 15°, a stair/cliff condition is flagged.

---

## 7. Actuator Control

### 7.1 Haptic (Vibration Motors)

Three ERM motors driven via GPIO. The `HapticController::activate()` method maps the hazard level to a vibration pattern:
- NONE → all off
- FAR / MEDIUM → centre motor only
- CLOSE / CRITICAL → all three motors

### 7.2 Audio (Buzzer)

Single GPIO pin drives the buzzer. Activated on falls, stair events, and CRITICAL obstacle proximity.

---

## 8. Error Handling

| Scenario                        | Strategy                                       |
|---------------------------------|------------------------------------------------|
| I2C read failure                | Retry up to 3 times, then log and skip         |
| Out-of-range sensor value       | Clamp to valid range, flag as unreliable        |
| GPIO init failure               | Log error and exit gracefully                   |
| Power loss                      | Motors default to OFF (fail-safe)               |

---

## 9. Build System

Uses CMake >= 3.10. Libraries are separated per module (`lidar_driver`, `imu_driver`, `haptic_controller`, `audio_controller`, `decision_engine`). Tests are integrated with `enable_testing()` and `add_subdirectory(test)`.

---

## 10. Latency Measurement

Timestamps can be added around the sensing–actuation pipeline to measure actual cycle times:

```cpp
auto t0 = std::chrono::high_resolution_clock::now();
// ... sensor read + classify + actuate ...
auto t1 = std::chrono::high_resolution_clock::now();
auto us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
std::cout << "Cycle: " << us << " us" << std::endl;
```

| Subsystem              | Expected      | Measured |
|------------------------|---------------|----------|
| LiDAR I2C read         | 2–5 ms        | TBD      |
| IMU I2C read           | 1–2 ms        | TBD      |
| Classification logic   | < 0.5 ms      | TBD      |
| GPIO motor activation  | < 0.1 ms      | TBD      |
| **Full cycle**         | **~9 ms**     | TBD      |

---

## 11. Documentation

All public functions use doxygen-style comments (`@brief`, `@param`, `@return`).
