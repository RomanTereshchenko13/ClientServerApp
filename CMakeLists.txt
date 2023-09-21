cmake_minimum_required (VERSION 3.12)

project("FileTransfer" CXX)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(Boost_USE_MULTITHREAD ON)
FIND_PACKAGE(Boost 1.82.0 REQUIRED COMPONENTS log_setup log system thread)

add_executable(client client/main.cpp client/client.cpp shared/logger.cpp)

target_include_directories(client
  PUBLIC
    ${Boost_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/client"
    "${CMAKE_CURRENT_SOURCE_DIR}/shared"
)

target_link_libraries(client
  PUBLIC
    Boost::log_setup
    Boost::log
    Boost::system
    Boost::thread
  )

target_compile_options(client
  PUBLIC
    -DBOOST_LOG_DYN_LINK
  ) 

target_precompile_headers(client
  PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/shared/pch.h"
  )

add_executable(server server/main.cpp server/server.cpp shared/logger.cpp)

target_include_directories(server
  PUBLIC
    ${Boost_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/server"
    "${CMAKE_CURRENT_SOURCE_DIR}/shared"
  )

target_link_libraries(server
  PUBLIC
    Boost::log_setup
    Boost::log
    Boost::system
    Boost::thread
  )

target_compile_options(server
  PUBLIC
    -DBOOST_LOG_DYN_LINK
  )

  target_precompile_headers(server
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/shared/pch.h"
  )
