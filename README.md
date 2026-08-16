# Distributed Key-Value Store

A simple distributed key-value store built with Berkeley sockets in C++.

## Features
- PUT / GET / DELETE over TCP
- Primary-replica replication
- Thread-per-client concurrency
- Mutex-protected shared storage

## Build & Run

```bash
mkdir build && cd build
cmake .. && make

./server replica

./server primary

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
