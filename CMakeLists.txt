cmake_minimum_required(VERSION 3.15)
project(ServerClientApp)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)

# Add executable for the server
add_executable(Server
    src/server/Server.cpp
    src/server/Server.h
    src/server/main.cpp
)

# Add executable for the client
add_executable(Client
    src/client/Client.cpp
    src/client/Client.h
    src/client/main.cpp
)

# Include directories for the project
target_include_directories(Server PRIVATE
    include
    src/shared
)

target_include_directories(Client PRIVATE
    include
    src/shared
)

# Set the output directory for the executables 
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)