#!/usr/bin/env bash
# Wait for the Feather, then build + flash.
# Usage: scripts/flash.sh [extra pio args...]
set -euo pipefail
cd "$(dirname "$0")/.."

PORT=$(scripts/_wait_for_board.sh)
echo "Found ${PORT}. Building & uploading..."
exec pio run -t upload --upload-port "$PORT" "$@"
