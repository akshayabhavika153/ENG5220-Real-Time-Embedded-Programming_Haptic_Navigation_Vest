#!/usr/bin/env bash
set -euo pipefail

action="${1:-}"
if [[ -z "$action" ]]; then
  echo "Usage: $0 {left_slow|left_fast|left_cont|right_slow|right_fast|right_cont|both_slow|both_fast|both_cont|stop}" >&2
  exit 1
fi

HAPTIC_CHIP="${HVEST_HAPTIC_CHIP:-gpiochip0}"
LEFT_LINE="${HVEST_LEFT_MOTOR_LINE:-23}"
RIGHT_LINE="${HVEST_RIGHT_MOTOR_LINE:-24}"
BACKEND="${HVEST_HAPTIC_BACKEND:-gpio}"
DRV_BUS="${HVEST_DRV2605L_BUS:-1}"
DRV_ADDR="${HVEST_DRV2605L_ADDR:-0x5a}"

run_drv() {
  if ! command -v i2cset >/dev/null 2>&1; then
    echo "i2cset not found. Install i2c-tools (sudo apt install i2c-tools)." >&2
    exit 2
  fi

  local profile="$1"
  local effect=10
  case "$profile" in
    slow) effect=10 ;;
    fast) effect=47 ;;
    danger) effect=82 ;;
    rapid) effect=47 ;;
    strong) effect=84 ;;
    stop)
      i2cset -y "$DRV_BUS" "$DRV_ADDR" 0x0c 0x00 >/dev/null
      return
      ;;
    *)
      echo "Unsupported DRV2605L profile: $profile" >&2
      exit 1
      ;;
  esac

  i2cset -y "$DRV_BUS" "$DRV_ADDR" 0x01 0x00 >/dev/null
  i2cset -y "$DRV_BUS" "$DRV_ADDR" 0x03 0x01 >/dev/null
  i2cset -y "$DRV_BUS" "$DRV_ADDR" 0x04 "$effect" >/dev/null
  i2cset -y "$DRV_BUS" "$DRV_ADDR" 0x05 0x00 >/dev/null
  i2cset -y "$DRV_BUS" "$DRV_ADDR" 0x0c 0x01 >/dev/null

  if [[ "$profile" == "rapid" ]]; then
    sleep 0.06
    i2cset -y "$DRV_BUS" "$DRV_ADDR" 0x0c 0x01 >/dev/null
  fi
}

set_motor() {
  local line="$1"
  local value="$2"
  if command -v gpioset >/dev/null 2>&1; then
    gpioset -c "$HAPTIC_CHIP" -t 0 "$line=$value"
    return
  fi

  echo "gpioset not found. Install gpiod tools (sudo apt install gpiod)." >&2
  exit 2
}

set_both() {
  local value="$1"
  set_motor "$LEFT_LINE" "$value"
  set_motor "$RIGHT_LINE" "$value"
}

pulse_one() {
  local line="$1"
  local on_s="$2"
  local off_s="$3"
  local on_ms
  on_ms="$(awk "BEGIN {printf \"%d\", $on_s * 1000}")"
  if command -v gpioset >/dev/null 2>&1; then
    gpioset -c "$HAPTIC_CHIP" -t "${on_ms}ms,0" "$line=1"
  else
    set_motor "$line" 1
    sleep "$on_s"
    set_motor "$line" 0
  fi
  sleep "$off_s"
}

pulse_both() {
  local on_s="$1"
  local off_s="$2"
  local on_ms
  on_ms="$(awk "BEGIN {printf \"%d\", $on_s * 1000}")"
  if command -v gpioset >/dev/null 2>&1; then
    gpioset -c "$HAPTIC_CHIP" -t "${on_ms}ms,0" "$LEFT_LINE=1" "$RIGHT_LINE=1"
  else
    set_both 1
    sleep "$on_s"
    set_both 0
  fi
  sleep "$off_s"
}

case "$action" in
  left_slow)
    if [[ "$BACKEND" == "drv2605l" ]]; then
      run_drv slow
      exit 0
    fi
    pulse_one "$LEFT_LINE" 0.15 0.20
    ;;
  left_fast)
    if [[ "$BACKEND" == "drv2605l" ]]; then
      run_drv fast
      exit 0
    fi
    pulse_one "$LEFT_LINE" 0.06 0.08
    pulse_one "$LEFT_LINE" 0.06 0.08
    ;;
  left_cont)
    if [[ "$BACKEND" == "drv2605l" ]]; then
      run_drv danger
      exit 0
    fi
    if command -v gpioset >/dev/null 2>&1; then
      gpioset -c "$HAPTIC_CHIP" -t "300ms,0" "$LEFT_LINE=1"
    else
      set_motor "$LEFT_LINE" 1
    fi
    ;;
  right_slow)
    if [[ "$BACKEND" == "drv2605l" ]]; then
      run_drv slow
      exit 0
    fi
    pulse_one "$RIGHT_LINE" 0.15 0.20
    ;;
  right_fast)
    if [[ "$BACKEND" == "drv2605l" ]]; then
      run_drv fast
      exit 0
    fi
    pulse_one "$RIGHT_LINE" 0.06 0.08
    pulse_one "$RIGHT_LINE" 0.06 0.08
    ;;
  right_cont)
    if [[ "$BACKEND" == "drv2605l" ]]; then
      run_drv danger
      exit 0
    fi
    if command -v gpioset >/dev/null 2>&1; then
      gpioset -c "$HAPTIC_CHIP" -t "300ms,0" "$RIGHT_LINE=1"
    else
      set_motor "$RIGHT_LINE" 1
    fi
    ;;
  both_slow)
    if [[ "$BACKEND" == "drv2605l" ]]; then
      run_drv slow
      exit 0
    fi
    pulse_both 0.15 0.20
    ;;
  both_fast)
    if [[ "$BACKEND" == "drv2605l" ]]; then
      run_drv rapid
      exit 0
    fi
    pulse_both 0.06 0.08
    pulse_both 0.06 0.08
    ;;
  both_cont)
    if [[ "$BACKEND" == "drv2605l" ]]; then
      run_drv strong
      exit 0
    fi
    if command -v gpioset >/dev/null 2>&1; then
      gpioset -c "$HAPTIC_CHIP" -t "300ms,0" "$LEFT_LINE=1" "$RIGHT_LINE=1"
    else
      set_both 1
    fi
    ;;
  stop)
    if [[ "$BACKEND" == "drv2605l" ]]; then
      run_drv stop
      exit 0
    fi
    set_both 0
    ;;
  *)
    echo "Unknown action: $action" >&2
    exit 1
    ;;
esac
