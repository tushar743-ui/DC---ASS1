cmake_minimum_required(VERSION 3.10)
project(distributed-kv-store)

set(CMAKE_CXX_STANDARD 17)

add_executable(server
    server/server.cpp
    server/storage.cpp
)
target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(server pthread)

add_executable(client
    client/client.cpp
)
target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR})