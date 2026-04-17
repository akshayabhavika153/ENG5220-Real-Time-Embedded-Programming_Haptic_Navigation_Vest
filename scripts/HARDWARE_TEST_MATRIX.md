# Haptic Navigation Vest Hardware Test Matrix

This test matrix gives exact input actions and expected outputs for each component path.

## 1) Prerequisites

```bash
cd /home/blindvest/Downloads/HapticVest
chmod +x scripts/*.sh
sudo apt update
sudo apt install -y gpiod i2c-tools cmake build-essential
```

Optional GPIO mapping (change to your wiring):

```bash
export HVEST_BUZZER_CHIP=gpiochip0
export HVEST_BUZZER_LINE=18
export HVEST_HAPTIC_CHIP=gpiochip0
export HVEST_LEFT_MOTOR_LINE=23
export HVEST_RIGHT_MOTOR_LINE=24
```

## 2) Build and run

```bash
cd /home/blindvest/Downloads/HapticVest
cmake -S . -B build
cmake --build build -j
source scripts/export_hvest_env.sh
./build/vest_app
```

Stop with Ctrl+C.

If your LiDAR UART path is different, override it at runtime:

```bash
export HVEST_LIDAR_DEVICE=/dev/ttyAMA0
```

If you want to keep integrated outputs active during bench checks even when LiDAR stream is unstable,
temporarily disable health timeout monitoring:

```bash
export HVEST_SENSOR_HEALTH_ENABLE=0
```

Or relax timeout thresholds instead:

```bash
export HVEST_SENSOR_STARTUP_TIMEOUT_MS=8000
export HVEST_SENSOR_RUNTIME_TIMEOUT_MS=5000
```

## 3) One-command full observation run

```bash
cd /home/blindvest/Downloads/HapticVest
./scripts/run_hvest_hardware_observe.sh --run-seconds 45
```

With battery simulation:

```bash
./scripts/run_hvest_hardware_observe.sh --simulate-battery 8 --run-seconds 25
```

## 4) Exact input and expected output

### A) Audio controller path

Input command:

```bash
./scripts/hvest_audio.sh slow
```

Expected terminal: no extra output if `gpioset` works; script exits 0.
Expected physical: buzzer short pulse (about 100 ms on, 200 ms off).

Input command:

```bash
./scripts/hvest_audio.sh fast
```

Expected physical: two short buzzer pulses.

Input command:

```bash
./scripts/hvest_audio.sh cont
```

Expected physical: buzzer stays ON until stop.

Input command:

```bash
./scripts/hvest_audio.sh stop
```

Expected physical: buzzer OFF.

### B) Haptic controller path

Input command:

```bash
./scripts/hvest_haptic.sh left_slow
```

Expected physical: left motor one slow pulse.

Input command:

```bash
./scripts/hvest_haptic.sh right_fast
```

Expected physical: right motor two fast pulses.

Input command:

```bash
./scripts/hvest_haptic.sh both_cont
```

Expected physical: both motors continuous vibration until stop.

Input command:

```bash
./scripts/hvest_haptic.sh stop
```

Expected physical: all motors OFF.

### C) LiDAR to navigation output path (current app wiring = AHEAD only)

Current code monitors front LiDAR only. Expected outputs are for both motors and audio pattern.

- If distance > 2.0 m:
  - Expected terminal: `[HAPTIC] No vibration`, `[AUDIO] No sound` (fallback mode)
  - Expected physical: no haptic, no beep
- If 1.0 m < distance <= 2.0 m:
  - Expected terminal: slow pattern
  - Expected physical: slow BOTH vibration + slow beep
- If 0.5 m < distance <= 1.0 m:
  - Expected terminal: fast pattern
  - Expected physical: fast BOTH vibration + fast beep
- If distance <= 0.5 m:
  - Expected terminal: continuous pattern
  - Expected physical: continuous BOTH vibration + continuous buzzer

### D) IMU fall-detection path

Input action on hardware:
- Create a large acceleration spike (fall-like event, jerk/drop test with safe rig).

Expected terminal:
- `[EMERGENCY] Fall detected`
- `[EMERGENCY] BEEP (1 sec)` twice
- `[EMERGENCY] Voice: Help needed`

Expected physical:
- Emergency buzzer pattern and emergency response (as configured in your output hardware).

Note: code applies a 5-second debounce between fall events.

### E) Battery monitor path

Input:

```bash
echo 8 > /tmp/hvest_bat
env HVEST_BATTERY_CAPACITY_PATH=/tmp/hvest_bat ./build/vest_app
```

Expected terminal:
- `[SYSTEM] Battery critical`
- `[AUDIO] Warning: Battery critical`

Expected physical:
- System low-battery warning behavior through audio path.

For low (not critical): use 15 instead of 8.

### F) Sensor health path

Input action:
- Keep expected front LiDAR disconnected or stop its data stream after startup.

Expected terminal:
- `[SYSTEM] Sensor error detected`
- `[AUDIO] Warning: Sensor error`
- Navigation output is stopped by priority manager.

Expected physical:
- No navigation vibration pattern after sensor error; warning output active.

## 5) Best execution flow for ENG5220 demo

1. Run actuator-only checks first (proves GPIO and wiring).
2. Run integrated app and demonstrate LiDAR distance bands.
3. Trigger battery warning (simulated file method).
4. Trigger IMU fall event (safe controlled test).
5. Show sensor fault behavior by disconnecting LiDAR data.
6. End with Ctrl+C clean shutdown.

This order demonstrates modular verification plus full system behavior, matching rubric expectations for real-time and safety checks.
