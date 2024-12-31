cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(BeeLight)

add_subdirectory(drivers)
add_subdirectory(src/events)
add_subdirectory(src/sensors)

include_directories(src/)

target_sources(app PRIVATE
                   src/main.c
)

target_include_directories(app PRIVATE
                               include
)
