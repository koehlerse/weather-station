# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion RISCV_RPI_2_0_0_5)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(weather-station C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(weather-station 
    weather-station.c
    wifi.c 
    bme280.c
    bh1750.c
    mq135.c
    mqtt_client.c
    sensors.c
    hardware.c
    rosc.c
    sleep.c
)

pico_set_program_name(weather-station "weather-station")
pico_set_program_version(weather-station "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(weather-station 0)
pico_enable_stdio_usb(weather-station 1)

# Add the standard library to the build
target_link_libraries(weather-station
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(weather-station PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(weather-station
        pico_stdlib
        hardware_dma
        hardware_adc
        hardware_pio
        hardware_interp
        hardware_timer
        hardware_watchdog
        hardware_clocks
        hardware_i2c
        hardware_pwm
        pico_lwip_mqtt
        pico_cyw43_arch_lwip_threadsafe_background
        pico_lwip_mbedtls
        pico_lwip
        pico_aon_timer
)

pico_add_extra_outputs(weather-station)

