#!/bin/bash

cd "$(dirname "$0")/.."
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release > /dev/null
make -j4

cd ..

echo "Starting replica server on port 8081..."
./build/server replica &
REPLICA_PID=$!
sleep 0.5

echo "Starting primary server on port 8080..."
./build/server primary &
PRIMARY_PID=$!

echo ""
echo "Primary PID : $PRIMARY_PID"
echo "Replica PID : $REPLICA_PID"
echo ""
echo "Press Ctrl+C to stop both servers."

trap "kill $PRIMARY_PID $REPLICA_PID 2>/dev/null; echo 'Servers stopped.'" INT
wait