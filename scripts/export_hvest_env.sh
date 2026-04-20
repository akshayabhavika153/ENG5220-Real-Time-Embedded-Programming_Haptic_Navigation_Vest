#!/usr/bin/env bash

# Only enable strict mode when this file is executed directly.
# When sourced, inheriting -e/-u can terminate the caller shell unexpectedly.
if [[ "${BASH_SOURCE[0]}" == "$0" ]]; then
	set -euo pipefail
fi

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

export HVEST_AUDIO_SLOW_CMD="$ROOT_DIR/scripts/hvest_audio.sh slow"
export HVEST_AUDIO_FAST_CMD="$ROOT_DIR/scripts/hvest_audio.sh fast"
export HVEST_AUDIO_CONTINUOUS_CMD="$ROOT_DIR/scripts/hvest_audio.sh cont"
export HVEST_AUDIO_STOP_CMD="$ROOT_DIR/scripts/hvest_audio.sh stop"

export HVEST_HAPTIC_SINGLE_SLOW_CMD="$ROOT_DIR/scripts/hvest_haptic_single.sh slow"
export HVEST_HAPTIC_SINGLE_FAST_CMD="$ROOT_DIR/scripts/hvest_haptic_single.sh fast"
export HVEST_HAPTIC_SINGLE_CONT_CMD="$ROOT_DIR/scripts/hvest_haptic_single.sh cont"
export HVEST_HAPTIC_SINGLE_STOP_CMD="$ROOT_DIR/scripts/hvest_haptic_single.sh stop"

# Backward-compatible aliases in case other scripts still reference old names.
export HVEST_HAPTIC_BOTH_SLOW_CMD="$HVEST_HAPTIC_SINGLE_SLOW_CMD"
export HVEST_HAPTIC_BOTH_FAST_CMD="$HVEST_HAPTIC_SINGLE_FAST_CMD"
export HVEST_HAPTIC_BOTH_CONT_CMD="$HVEST_HAPTIC_SINGLE_CONT_CMD"
export HVEST_HAPTIC_STOP_CMD="$HVEST_HAPTIC_SINGLE_STOP_CMD"

echo "Single-hardware command environment exported."
echo "Set these if needed:"
echo "  export HVEST_LIDAR_DEVICE=/dev/serial0"
echo "  export HVEST_HAPTIC_CHIP=gpiochip0"
echo "  export HVEST_MOTOR_LINE=23"
echo "  export HVEST_BUZZER_CHIP=gpiochip0"
echo "  export HVEST_BUZZER_LINE=18"
echo "Recommended DRV2605L backend for your current hardware:"
echo "  export HVEST_HAPTIC_BACKEND=drv2605l"
echo "  export HVEST_DRV2605L_BUS=1"
echo "  export HVEST_DRV2605L_ADDR=0x5a"
