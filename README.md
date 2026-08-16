# Distributed Key-Value Store

A simple distributed key-value store built with Berkeley sockets in C++.

## Features
- PUT / GET / DELETE over TCP
- Primary-replica replication
- Thread-per-client concurrency
- Mutex-protected shared storage

## Build & Run

```bash
# Build
mkdir build && cd build
cmake .. && make

# Terminal 1 — start replica first
./server replica

# Terminal 2 — start primary
./server primary

# Terminal 3 — run client
./client
```

## Example Session

```
kv> PUT city Nagpur
Response: OK

kv> GET city
Response: VALUE Nagpur

kv> DELETE city
Response: OK

kv> GET city
Response: NOT_FOUND
```

## Architecture

```
Client → Primary Server (port 8080)
                |
                | TCP replication (writes only)
                v
         Replica Server (port 8081)
```
