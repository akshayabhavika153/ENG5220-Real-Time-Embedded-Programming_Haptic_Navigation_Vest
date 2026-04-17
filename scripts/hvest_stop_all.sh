#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

# Release any lingering GPIO holders first.
pkill gpioset >/dev/null 2>&1 || true

# Best-effort explicit off commands.
"$ROOT_DIR/scripts/hvest_audio.sh" stop || true
"$ROOT_DIR/scripts/hvest_haptic.sh" stop || true

# Stop app instances if still running.
pkill -f "$ROOT_DIR/build/vest_app" >/dev/null 2>&1 || true
pkill -f "$ROOT_DIR/vest_app" >/dev/null 2>&1 || true

echo "HVEST outputs and app stop commands sent."
