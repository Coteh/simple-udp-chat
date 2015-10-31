# require Cmake 2.8+
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# initial setup
SET(appName simple-udp-chat)
project(${appName})

# checking for C++11/C++0x support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(WARNING "Compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# other compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")

include_directories(${${appName}_SOURCE_DIR}/Shared/include)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "Shared/lib/")
add_library(${appName}_shared_stuff SHARED Shared/src/cipher.cpp)

set(BUILD_CLIENT CACHE BOOL true)
set(BUILD_SERVER CACHE BOOL true)
# include directories
if(BUILD_CLIENT)
  include_directories(${${appName}_SOURCE_DIR}/Client/include)
endif(BUILD_CLIENT)
if(BUILD_SERVER)
  include_directories(${${appName}_SOURCE_DIR}/Server/include)
endif(BUILD_SERVER)

# add definitions
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

# add targets
if(BUILD_CLIENT)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "Client/bin/")
  #add_subdirectory(Client)
  add_executable(${appName}_client Client/src/main.cpp Client/include/Client.h Client/src/Client.cpp)
  target_link_libraries(${appName}_client ${appName}_shared_stuff)
endif(BUILD_CLIENT)
if(BUILD_SERVER)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "Server/bin/")
  #add_subdirectory(Server)
  add_executable(${appName}_server Server/src/main.cpp Server/include/Server.h Server/src/Server.cpp)
  target_link_libraries(${appName}_server ${appName}_shared_stuff)
endif(BUILD_SERVER)
