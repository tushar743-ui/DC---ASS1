#!/usr/bin/env bash
set -euo pipefail

if [ $# -lt 1 ]; then
  echo "Usage: $0 \"GET key\" | \"PUT key value\" | \"DELETE key\" [port]"
  exit 1
fi

PORT="${2:-5001}"
./build/client "$1" "$PORT"
