#!/usr/bin/env bash
set -euo pipefail

./build/server 5001 &
./build/server 5002 &

wait
