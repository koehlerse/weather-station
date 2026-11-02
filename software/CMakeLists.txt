cmake_minimum_required(VERSION 4.3)

include(pico_sdk_import.cmake)

project(weather-station-software C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(weather-station-software
        src/main.c
)

target_link_libraries(weather-station-software
    pico_stdlib
)

pico_add_extra_outputs(weather-station-software)
