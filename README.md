# ClientServerApp

This repository contains an example of a simple TCP Client-Server application using Boost Asio. The project is organized into a client and a server application, along with shared code.

# Features

- Uses Boost Asio for networking
- Built with CMake
- Includes logging with Boost Log

# Requirements

- C++17 compiler
- CMake 3.12 or higher
- Boost Libraries (Asio, Log, etc.)
- Ubuntu 22.04

# Building the Project

## Clone the repository.

    git clone https://github.com/RomanTereshchenko13/ClientServerApp

## Navigate to the project directory.

    cd 

## Create a build directory and navigate into it.

    mkdir build
    cd build

## Run CMake to generate the build files.

    cmake ..

## Compile the project.

    make

You will find the client and server executables in the build directory.

# Usage

## Server

To run the server:

    ./server

## Client

To run the client:

    ./client <server_address>

Replace <server_address> with the IP address or domain name of the server.
