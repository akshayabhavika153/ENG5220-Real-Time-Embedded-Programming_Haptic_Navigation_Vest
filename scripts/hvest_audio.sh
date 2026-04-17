#!/usr/bin/env bash
set -euo pipefail

action="${1:-}"
if [[ -z "$action" ]]; then
  echo "Usage: $0 {slow|fast|cont|stop}" >&2
  exit 1
fi

BUZZER_CHIP="${HVEST_BUZZER_CHIP:-gpiochip0}"
BUZZER_LINE="${HVEST_BUZZER_LINE:-18}"

set_buzzer() {
  local value="$1"
  if command -v gpioset >/dev/null 2>&1; then
    # Force a short, deterministic set then release so line state applies and command returns.
    gpioset -c "$BUZZER_CHIP" -t "20ms,0" "$BUZZER_LINE=$value"
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
    gpioset -c "$BUZZER_CHIP" -t "${on_ms}ms,0" "$BUZZER_LINE=1"
  else
    set_buzzer 1
    sleep "$on_s"
    set_buzzer 0
  fi
  sleep "$off_s"
}

case "$action" in
  slow)
    pulse 0.10 0.20
    ;;
  fast)
    pulse 0.05 0.08
    pulse 0.05 0.08
    ;;
  cont)
    # Use a finite hold so the command returns; repeated calls create continuous feel.
    if command -v gpioset >/dev/null 2>&1; then
      gpioset -c "$BUZZER_CHIP" -t "300ms,0" "$BUZZER_LINE=1"
    else
      set_buzzer 1
    fi
    ;;
  stop)
    set_buzzer 0
    ;;
  *)
    echo "Unknown action: $action" >&2
    exit 1
    ;;
esac
