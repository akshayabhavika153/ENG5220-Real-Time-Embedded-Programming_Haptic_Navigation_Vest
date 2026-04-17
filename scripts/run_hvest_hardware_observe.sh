#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
APP_BIN="${HVEST_APP_BIN:-$ROOT_DIR/build/vest_app}"
RUN_SECONDS=30
SIM_BATTERY=""

usage() {
  cat <<'EOF'
Usage:
  run_hvest_hardware_observe.sh [--run-seconds N] [--simulate-battery PERCENT]

Examples:
  ./scripts/run_hvest_hardware_observe.sh --run-seconds 45
  ./scripts/run_hvest_hardware_observe.sh --simulate-battery 8 --run-seconds 20

What it does:
  1) Exports haptic/audio command hooks
  2) Runs actuator-only checks (audio + haptic scripts)
  3) Runs the main vest app for observation
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --run-seconds)
      RUN_SECONDS="$2"
      shift 2
      ;;
    --simulate-battery)
      SIM_BATTERY="$2"
      shift 2
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "Unknown argument: $1" >&2
      usage
      exit 1
      ;;
  esac
done

if [[ ! -x "$APP_BIN" ]]; then
  echo "App binary not found: $APP_BIN" >&2
  echo "Build first:" >&2
  echo "  cd $ROOT_DIR" >&2
  echo "  cmake -S . -B build" >&2
  echo "  cmake --build build -j" >&2
  exit 2
fi

if [[ ! -x "$ROOT_DIR/scripts/hvest_audio.sh" || ! -x "$ROOT_DIR/scripts/hvest_haptic.sh" ]]; then
  echo "Actuator scripts are not executable. Fix with:" >&2
  echo "  chmod +x $ROOT_DIR/scripts/*.sh" >&2
  exit 3
fi

source "$ROOT_DIR/scripts/export_hvest_env.sh"

cleanup() {
  if [[ -n "${HVEST_BATTERY_CAPACITY_PATH:-}" && -n "${TMP_BAT_FILE:-}" && -f "$TMP_BAT_FILE" ]]; then
    rm -f "$TMP_BAT_FILE"
    unset HVEST_BATTERY_CAPACITY_PATH
  fi
}
trap cleanup EXIT

echo ""
echo "=== Step 1: Audio-only path test ==="
"$ROOT_DIR/scripts/hvest_audio.sh" slow
"$ROOT_DIR/scripts/hvest_audio.sh" fast
if command -v timeout >/dev/null 2>&1; then
  timeout 1s "$ROOT_DIR/scripts/hvest_audio.sh" cont || true
else
  "$ROOT_DIR/scripts/hvest_audio.sh" cont &
  CONT_PID=$!
  sleep 1
  kill -TERM "$CONT_PID" 2>/dev/null || true
  wait "$CONT_PID" 2>/dev/null || true
fi
"$ROOT_DIR/scripts/hvest_audio.sh" stop

echo ""
echo "=== Step 2: Haptic-only path test ==="
"$ROOT_DIR/scripts/hvest_haptic.sh" left_slow
"$ROOT_DIR/scripts/hvest_haptic.sh" left_fast
"$ROOT_DIR/scripts/hvest_haptic.sh" right_slow
"$ROOT_DIR/scripts/hvest_haptic.sh" right_fast
"$ROOT_DIR/scripts/hvest_haptic.sh" both_slow
"$ROOT_DIR/scripts/hvest_haptic.sh" both_fast
if command -v timeout >/dev/null 2>&1; then
  timeout 1s "$ROOT_DIR/scripts/hvest_haptic.sh" both_cont || true
else
  "$ROOT_DIR/scripts/hvest_haptic.sh" both_cont &
  CONT_PID=$!
  sleep 1
  kill -TERM "$CONT_PID" 2>/dev/null || true
  wait "$CONT_PID" 2>/dev/null || true
fi
"$ROOT_DIR/scripts/hvest_haptic.sh" stop

if [[ -n "$SIM_BATTERY" ]]; then
  TMP_BAT_FILE="$(mktemp)"
  printf '%s\n' "$SIM_BATTERY" > "$TMP_BAT_FILE"
  export HVEST_BATTERY_CAPACITY_PATH="$TMP_BAT_FILE"
  echo ""
  echo "Battery simulation enabled: ${SIM_BATTERY}%"
  echo "HVEST_BATTERY_CAPACITY_PATH=$HVEST_BATTERY_CAPACITY_PATH"
fi

echo ""
echo "=== Step 3: Integrated app run (${RUN_SECONDS}s) ==="
echo "Observe terminal logs and physical outputs."
echo "Saving full terminal output to: $ROOT_DIR/run.log"

if command -v timeout >/dev/null 2>&1; then
  timeout "${RUN_SECONDS}s" "$APP_BIN" | tee "$ROOT_DIR/run.log" || true
else
  "$APP_BIN" | tee "$ROOT_DIR/run.log" &
  APP_PID=$!
  sleep "$RUN_SECONDS"
  kill -TERM "$APP_PID" 2>/dev/null || true
  wait "$APP_PID" 2>/dev/null || true
fi

echo ""
echo "Completed. Review logs in $ROOT_DIR/run.log"
