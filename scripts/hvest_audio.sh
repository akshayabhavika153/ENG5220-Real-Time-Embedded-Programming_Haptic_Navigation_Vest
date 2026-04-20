#!/usr/bin/env bash
set -euo pipefail

action="${1:-}"
if [[ -z "$action" ]]; then
  echo "Usage: $0 {slow|fast|cont|stop}" >&2
  exit 1
fi

BUZZER_CHIP="${HVEST_BUZZER_CHIP:-gpiochip0}"
BUZZER_LINE="${HVEST_BUZZER_LINE:-18}"
PID_FILE="${HVEST_AUDIO_CONT_PID_FILE:-/tmp/hvest_audio_cont.pid}"
BUZZER_ACTIVE_LOW="${HVEST_BUZZER_ACTIVE_LOW:-0}"

if [[ "$BUZZER_ACTIVE_LOW" == "1" ]]; then
  BUZZER_ON_VALUE=0
  BUZZER_OFF_VALUE=1
else
  BUZZER_ON_VALUE=1
  BUZZER_OFF_VALUE=0
fi

gpioset_safe() {
  local timing="$1"
  local assignment="$2"
  local tries=0

  while (( tries < 8 )); do
    if gpioset -c "$BUZZER_CHIP" -t "$timing" "$assignment" 2>/dev/null; then
      return 0
    fi
    tries=$((tries + 1))
    sleep 0.05
  done

  echo "gpioset: unable to request line $BUZZER_LINE on $BUZZER_CHIP after retries" >&2
  return 1
}

is_pid_running() {
  local pid="$1"
  [[ -n "$pid" ]] && kill -0 "$pid" >/dev/null 2>&1
}

stop_cont_loop() {
  if [[ -f "$PID_FILE" ]]; then
    local pid
    pid="$(cat "$PID_FILE" 2>/dev/null || true)"
    if is_pid_running "$pid"; then
      kill "$pid" >/dev/null 2>&1 || true
      wait "$pid" 2>/dev/null || true
    fi
    rm -f "$PID_FILE"
    # Give any in-flight gpioset a short window to release the line.
    sleep 0.06
  fi
}

start_cont_loop() {
  if [[ -f "$PID_FILE" ]]; then
    local pid
    pid="$(cat "$PID_FILE" 2>/dev/null || true)"
    if is_pid_running "$pid"; then
      return
    fi
    rm -f "$PID_FILE"
  fi

  "$0" _cont_loop >/dev/null 2>&1 &
  echo "$!" > "$PID_FILE"
}

set_buzzer() {
  local value="$1"
  if command -v gpioset >/dev/null 2>&1; then
    # Force a short, deterministic set then release so line state applies and command returns.
    gpioset_safe "20ms,0" "$BUZZER_LINE=$value"
    return
  fi

  echo "gpioset not found. Install gpiod tools (sudo apt install gpiod)." >&2
  exit 2
}

pulse() {
  local on_s="$1"
  local off_s="$2"
  local on_ms
  on_ms="$(awk "BEGIN {printf \"%d\", $on_s * 1000}")"
  if command -v gpioset >/dev/null 2>&1; then
    gpioset_safe "${on_ms}ms,0" "$BUZZER_LINE=$BUZZER_ON_VALUE"
  else
    set_buzzer "$BUZZER_ON_VALUE"
    sleep "$on_s"
    set_buzzer "$BUZZER_OFF_VALUE"
  fi
  sleep "$off_s"
}

case "$action" in
  slow)
    stop_cont_loop
    pulse 0.10 0.20
    ;;
  fast)
    stop_cont_loop
    pulse 0.05 0.08
    pulse 0.05 0.08
    ;;
  cont)
    start_cont_loop
    ;;
  stop)
    stop_cont_loop
    set_buzzer "$BUZZER_OFF_VALUE"
    ;;
  _cont_loop)
    while true; do
      if command -v gpioset >/dev/null 2>&1; then
        gpioset_safe "350ms,0" "$BUZZER_LINE=$BUZZER_ON_VALUE" || true
      else
        set_buzzer "$BUZZER_ON_VALUE"
        sleep 0.35
        set_buzzer "$BUZZER_OFF_VALUE"
      fi
      sleep 0.02
    done
    ;;
  *)
    echo "Unknown action: $action" >&2
    exit 1
    ;;
esac
