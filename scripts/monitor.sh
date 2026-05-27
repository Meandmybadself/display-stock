#!/usr/bin/env bash
# Wait for the Feather, then open the serial monitor.
# Usage: scripts/monitor.sh
set -euo pipefail
cd "$(dirname "$0")/.."

PORT=$(scripts/_wait_for_board.sh)
echo "Found ${PORT}. Starting monitor (Ctrl-C to exit)..."
exec pio device monitor -p "$PORT" -b 115200
