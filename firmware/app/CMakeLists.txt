cmake_minimum_required(VERSION 3.20.0)

# Apply Zephyr patches before build
file(GLOB_RECURSE files RELATIVE ${CMAKE_SOURCE_DIR} "patches/zephyr/*.patch")
foreach(file ${files})
    message("Execute patch ${file}")
    execute_process(COMMAND 
                    git apply --directory=$ENV{ZEPHYR_BASE} ${CMAKE_CURRENT_SOURCE_DIR}/${file} --unsafe-paths)
endforeach()

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

string(REGEX MATCH "beelight_nrf54l15_cpuapp(_ns)?@([0-9]+)$" BEELIGHT_BOARD "${BOARD}")
if(BEELIGHT_BOARD)
    string(CONCAT VERSION_STRING "\"" ${CMAKE_MATCH_2} "\"")
    set(CONFIG_BEELIGHT_PCB_REV ${CMAKE_MATCH_2} CACHE INTERNAL "")

    message("PCB revision: " ${CONFIG_BEELIGHT_PCB_REV})
endif()

project(BeeLight)

add_subdirectory(src/events)
add_subdirectory(src/sensors)

include_directories(src)

target_sources(app PRIVATE
                   src/main.c
)

target_include_directories(app PRIVATE
                               src/sensors
                               src/events
)