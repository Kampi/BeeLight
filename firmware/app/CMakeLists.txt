cmake_minimum_required(VERSION 3.20.0)

# Apply Zephyr patches before build
file(GLOB_RECURSE files RELATIVE ${CMAKE_SOURCE_DIR} "patches/zephyr/*.patch")
foreach(file ${files})
    message("Execute patch ${file}")
    execute_process(COMMAND 
                    patch -p1 -d $ENV{ZEPHYR_BASE} -i ${CMAKE_CURRENT_SOURCE_DIR}/${file} -r - --no-backup-if-mismatch)
endforeach()

# Apply ncs-zigbee patches before build
file(GLOB_RECURSE files RELATIVE ${CMAKE_SOURCE_DIR} "patches/ncs-zigbee/*.patch")
foreach(file ${files})
    message("Execute patch ${file}")
    execute_process(COMMAND 
                    patch -p1 -d ${CMAKE_CURRENT_SOURCE_DIR}/../ncs-zigbee -i ${CMAKE_CURRENT_SOURCE_DIR}/${file} -r - --no-backup-if-mismatch --binary)
endforeach()

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

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
