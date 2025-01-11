cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(BeeLight)

# Get the target hardware
string(REGEX MATCH "beelight_nrf5340_cpuapp(_ns)?@([0-9]+)$" BEELIGHT_BOARD "${BOARD}")
if(BEELIGHT_BOARD)
    string(CONCAT VERSION_STRING "\"" ${CMAKE_MATCH_2} "\"")
    set(CONFIG_BEELIGHT_PCB_REV ${CMAKE_MATCH_2} CACHE INTERNAL "")
    set(CONFIG_BT_DIS_HW_REV_STR ${VERSION_STRING} CACHE INTERNAL "")

    message("PCB revision: " ${CONFIG_BEELIGHT_PCB_REV})
endif()

add_subdirectory(drivers)
add_subdirectory(src/events)
add_subdirectory(src/sensors)

include_directories(src/)

target_sources(app PRIVATE
                   src/main.c
)

target_include_directories(app PRIVATE
                               src/sensors
                               src/events
)
