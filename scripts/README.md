# Haptic Vest Hardware Scripts

These scripts connect the C++ app output events to GPIO actions for buzzer and haptic motors.

## 1) Install required tools

```bash
sudo apt update
sudo apt install -y gpiod
```

## 2) Make scripts executable

```bash
cd /home/blindvest/Downloads/HapticVest
chmod +x scripts/*.sh
```

## 3) (Optional) Set your real GPIO lines

```bash
export HVEST_BUZZER_CHIP=gpiochip0
export HVEST_BUZZER_LINE=18
export HVEST_HAPTIC_CHIP=gpiochip0
export HVEST_LEFT_MOTOR_LINE=23
export HVEST_RIGHT_MOTOR_LINE=24
```

These defaults match your working setup in `/home/blindvest/tfmini_test/actuator_test.sh`.

## 3b) Use DRV2605L backend (if your motors are on DRV2605L)

```bash
export HVEST_HAPTIC_BACKEND=drv2605l
export HVEST_DRV2605L_BUS=1
export HVEST_DRV2605L_ADDR=0x5a
```

Install i2c tools if needed:

```bash
sudo apt install -y i2c-tools
```

## 4) Export app command hooks

```bash
source /home/blindvest/Downloads/HapticVest/scripts/export_hvest_env.sh
```

## 5) Quick manual test

```bash
/home/blindvest/Downloads/HapticVest/scripts/hvest_audio.sh slow
/home/blindvest/Downloads/HapticVest/scripts/hvest_audio.sh fast
/home/blindvest/Downloads/HapticVest/scripts/hvest_haptic.sh left_fast
/home/blindvest/Downloads/HapticVest/scripts/hvest_haptic.sh right_fast
/home/blindvest/Downloads/HapticVest/scripts/hvest_haptic.sh stop
```

For a complete observation run (actuator checks + integrated app), use:

```bash
cd /home/blindvest/Downloads/HapticVest
./scripts/run_hvest_hardware_observe.sh --run-seconds 45
```

Detailed component input/output expectations are in:

```bash
scripts/HARDWARE_TEST_MATRIX.md
```

## 6) Run the application

```bash
cd /home/blindvest/Downloads/HapticVest
./vest_app
```

## 7) Clean CMake build and launch flow

Install CMake and compiler tools if needed:

```bash
sudo apt update
sudo apt install -y cmake build-essential
```

Build the project:

```bash
cd /home/blindvest/Downloads/HapticVest
cmake -S . -B build
cmake --build build -j
```

Run with hardware command hooks:

```bash
cd /home/blindvest/Downloads/HapticVest
source scripts/export_hvest_env.sh
./build/vest_app
```

Stop with Ctrl+C.

If you still get no output, check power stage and wiring:
- Use transistor/MOSFET driver for buzzer/motor current
- Common GND between board and driver
- Correct GPIO chip/line numbers
- Correct polarity and supply voltage
