cmake_minimum_required(VERSION 3.6)
project(FileMonitor)

set(CMAKE_CXX_STANDARD 20)

add_executable(FileMonitor
        main.cpp
        FileMonitor.h
        FileMonitorLinux.cpp
        FileMonitorLinux.h
        Factory.cpp
)