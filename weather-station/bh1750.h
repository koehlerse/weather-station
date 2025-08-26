#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

// I2C address of the BH1750
#define BH1750_I2C_ADDRESS 0x23

// BH1750 commands
#define BH1750_POWER_DOWN 0x00
#define BH1750_POWER_ON 0x01
#define BH1750_RESET 0x07
#define BH1750_CONTINUOUS_HIGH_RES_MODE 0x10
#define BH1750_ONE_TIME_HIGH_RES_MODE 0x20

// I2C pins for Pico W
#define I2C_PORT i2c0
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5

// Function to write a command byte to the BH1750
void bh1750_write(uint8_t command);

// Function to read the lux value from the BH1750
uint16_t bh1750_read_lux();