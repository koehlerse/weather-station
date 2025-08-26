#include "bh1750.h"

// Function to write a command byte to the BH1750
void bh1750_write(uint8_t command)
{
    i2c_write_blocking(I2C_PORT, BH1750_I2C_ADDRESS, &command, 1, false);
}

// Function to read the lux value from the BH1750
uint16_t bh1750_read_lux()
{
    uint8_t buffer[2];
    uint16_t lux;

    // Read 2 bytes from the sensor
    i2c_read_blocking(I2C_PORT, BH1750_I2C_ADDRESS, buffer, 2, false);

    // Combine the two bytes into a 16-bit integer
    lux = (buffer[0] << 8) | buffer[1];

    // The datasheet for high-resolution mode states the value is in lux directly.
    return lux;
}