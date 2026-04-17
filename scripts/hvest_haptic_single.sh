#!/usr/bin/env bash
set -euo pipefail

MODE="${1:-stop}"
BACKEND="${HVEST_HAPTIC_BACKEND:-drv2605l}"
HAPTIC_CHIP="${HVEST_HAPTIC_CHIP:-gpiochip0}"
MOTOR_LINE="${HVEST_MOTOR_LINE:-23}"
DRV_BUS="${HVEST_DRV2605L_BUS:-1}"
DRV_ADDR="${HVEST_DRV2605L_ADDR:-0x5a}"

write_reg() {
  i2cset -y "$DRV_BUS" "$DRV_ADDR" "$1" "$2" >/dev/null
}

check_drv_present() {
  if ! command -v i2cget >/dev/null 2>&1; then
    echo "i2cget not found. Install i2c-tools." >&2
    exit 2
  fi
  i2cget -y "$DRV_BUS" "$DRV_ADDR" 0x00 >/dev/null
}

init_drv() {
  # Mode: internal trigger
  write_reg 0x01 0x00
  # Library selection: strong ERM effects library
  write_reg 0x03 0x01
  # Waveform slots 2-8 off
  write_reg 0x05 0x00
  write_reg 0x06 0x00
  write_reg 0x07 0x00
  write_reg 0x08 0x00
  write_reg 0x09 0x00
  write_reg 0x0A 0x00
  write_reg 0x0B 0x00
}

trigger_effect_once() {
  local effect="$1"
  init_drv
  # Waveform slot 1 effect id
  write_reg 0x04 "$effect"
  # Go
  write_reg 0x0C 0x01
}

run_drv() {
  if ! command -v i2cset >/dev/null 2>&1; then
    echo "i2cset not found. Install i2c-tools." >&2
    exit 2
  fi

  check_drv_present

  case "$MODE" in
    slow)
      # soft click / light pulse
      trigger_effect_once 0x0A
      ;;
    fast)
      # stronger short pulse
      trigger_effect_once 0x2F
      ;;
    cont)
      # repeated stronger pulses; controller rate-limits this script anyway
      for _ in 1 2 3; do
        trigger_effect_once 0x47
        sleep 0.12
      done
      ;;
    stop)
      write_reg 0x0C 0x00
      ;;
    *)
      echo "Usage: $0 {slow|fast|cont|stop}" >&2
      exit 1
      ;;
  esac
}

pulse_gpio() {
  local on_ms="$1"
  if ! command -v gpioset >/dev/null 2>&1; then
    echo "gpioset not found. Install gpiod." >&2
    exit 2
  fi
  gpioset -c "$HAPTIC_CHIP" -t "${on_ms}ms,0" "$MOTOR_LINE=1"
}

run_gpio() {
  case "$MODE" in
    slow) pulse_gpio 150 ;;
    fast) pulse_gpio 70; sleep 0.08; pulse_gpio 70 ;;
    cont) pulse_gpio 320 ;;
    stop)
      if command -v gpioset >/dev/null 2>&1; then
        gpioset -c "$HAPTIC_CHIP" -t "20ms,0" "$MOTOR_LINE=0" || true
      fi
      ;;
    *)
      echo "Usage: $0 {slow|fast|cont|stop}" >&2
      exit 1
      ;;
  esac
}

case "$BACKEND" in
  drv2605l) run_drv ;;
  gpio) run_gpio ;;
  *)
    echo "Unknown HVEST_HAPTIC_BACKEND='$BACKEND'. Use 'drv2605l' or 'gpio'." >&2
    exit 1
    ;;
esac
