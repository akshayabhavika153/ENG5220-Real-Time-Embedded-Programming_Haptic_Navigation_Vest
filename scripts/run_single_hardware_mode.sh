#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${HVEST_BUILD_DIR:-$ROOT_DIR/build_local}"
APP_BIN="${HVEST_APP_BIN:-$BUILD_DIR/vest_app}"
RUN_SECONDS="${1:-30}"

cd "$ROOT_DIR"
cmake -S . -B "$BUILD_DIR" >/dev/null
cmake --build "$BUILD_DIR" -j >/dev/null
chmod +x "$ROOT_DIR"/scripts/*.sh
source "$ROOT_DIR/scripts/export_hvest_env.sh"

echo "[TEST] Speaker path"
"$ROOT_DIR/scripts/hvest_audio.sh" slow
"$ROOT_DIR/scripts/hvest_audio.sh" fast

echo "[TEST] Single motor path"
"$ROOT_DIR/scripts/hvest_haptic_single.sh" slow
"$ROOT_DIR/scripts/hvest_haptic_single.sh" fast

echo "[RUN] Starting vest_app for ${RUN_SECONDS}s"
if command -v timeout >/dev/null 2>&1; then
  timeout -k 2s "${RUN_SECONDS}s" "$APP_BIN" || true
else
  "$APP_BIN" &
  APP_PID=$!
  sleep "$RUN_SECONDS"
  if kill -0 "$APP_PID" >/dev/null 2>&1; then
    kill "$APP_PID" >/dev/null 2>&1 || true
    sleep 1
    kill -9 "$APP_PID" >/dev/null 2>&1 || true
    wait "$APP_PID" 2>/dev/null || true
  fi
fi
