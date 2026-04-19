# HapticVest - single hardware mode

This version is modified for the hardware you currently have:

- 1 x TF Mini S LiDAR
- 1 x speaker/buzzer path
- 1 x vibration motor

## What changed

- Removed IMU, battery, and sensor-health dependencies from the runtime path.
- Kept the modular C++ structure.
- Converted navigation output to a **single front-warning pattern**.
- Added output rate limiting so the motor/speaker are not retriggered on every LiDAR frame.
- Added LiDAR-based fall detection trigger:
  - rapid drop from >= 1.2 m to <= 0.45 m within 700 ms
  - 8 s cooldown to avoid repeated emergency triggers
  - emergency output drives continuous speaker + vibration alerts
- Added single-motor command hooks:
  - `HVEST_HAPTIC_SINGLE_SLOW_CMD`
  - `HVEST_HAPTIC_SINGLE_FAST_CMD`
  - `HVEST_HAPTIC_SINGLE_CONT_CMD`
  - `HVEST_HAPTIC_SINGLE_STOP_CMD`

## Distance mapping

- `> 2.0 m` -> stop
- `1.0 m to 2.0 m` -> slow warning
- `0.5 m to 1.0 m` -> fast warning
- `< 0.5 m` -> continuous warning

## Build

```bash
cd HapticVest_single_hardware
cmake -S . -B build
cmake --build build -j
```

## Run

```bash
chmod +x scripts/*.sh
source scripts/export_hvest_env.sh
./build/vest_app
```

Or use the helper:

```bash
./scripts/run_single_hardware_mode.sh 30
```

## Wiring assumptions

### TF Mini S
- VCC -> 5V
- GND -> GND
- TX -> Pi RX (GPIO 15 / serial RX)
- RX -> Pi TX (GPIO 14 / serial TX) if needed

### Single vibration motor
- Controlled through a transistor / driver stage from one GPIO line
- Default line in scripts: `GPIO 23`

### Speaker / buzzer
- Controlled by the existing `hvest_audio.sh` path
- Default line in scripts: `GPIO 18`

## Notes

- This build is intended for your **current hardware only**.
- It behaves as a **front obstacle warning vest**, not left/right guidance.
- If you later add more motors or sensors, you can expand from this baseline.


## Verified single-hardware configuration

This project is configured for the hardware currently on hand:

- 1 x TF Mini S LiDAR on UART
- 1 x DRV2605L haptic driver on I2C
- 1 x ERM vibration motor connected to the DRV2605L JST motor port
- 1 x speaker / buzzer driven through the existing audio script

### Required environment

```bash
source scripts/export_hvest_env.sh
export HVEST_HAPTIC_BACKEND=drv2605l
export HVEST_DRV2605L_BUS=1
export HVEST_DRV2605L_ADDR=0x5a
```

### I2C sanity check

Before running the app, verify both devices are visible:

```bash
i2cdetect -y 1
```

Expected addresses:

- `5a` for DRV2605L
- `6a` or `6b` for the LSM6DSOX IMU if it is connected

### Direct haptic test

```bash
./scripts/hvest_haptic_single.sh slow
./scripts/hvest_haptic_single.sh fast
./scripts/hvest_haptic_single.sh cont
```
