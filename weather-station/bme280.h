#pragma once

#include <stdbool.h>
#include "hardware/i2c.h"

// operating mode
typedef enum
{
    BME280_MODE_SLEEP = 0x00,
    BME280_MODE_FORCED = 0x01,
    BME280_MODE_NORMAL = 0x03
} bme280_mode_t;

// oversampling
typedef enum
{
    BME280_OVERSAMPLING_SKIP = 0x00,
    BME280_OVERSAMPLING_1X = 0x01,
    BME280_OVERSAMPLING_2X = 0x02,
    BME280_OVERSAMPLING_4X = 0x03,
    BME280_OVERSAMPLING_8X = 0x04,
    BME280_OVERSAMPLING_16X = 0x05
} bme280_oversampling_t;

// standby time in normal mode
typedef enum
{
    BME280_STANDBY_0_5_MS = 0x00,
    BME280_STANDBY_62_5_MS = 0x01,
    BME280_STANDBY_125_MS = 0x02,
    BME280_STANDBY_250_MS = 0x03,
    BME280_STANDBY_500_MS = 0x04,
    BME280_STANDBY_1000_MS = 0x05,
    BME280_STANDBY_10_MS = 0x06,
    BME280_STANDBY_20_MS = 0x07
} bme280_standby_t;

// iir filter oversampling mode
typedef enum
{
    BME280_FILTER_OFF = 0x00,
    BME280_FILTER_2 = 0x01,
    BME280_FILTER_4 = 0x02,
    BME280_FILTER_8 = 0x03,
    BME280_FILTER_16 = 0x04
} bme280_filter_t;

// config
typedef struct
{
    i2c_inst_t *i2c;
    uint8_t address;
    bme280_mode_t mode;
    bme280_oversampling_t osrs_t;
    bme280_oversampling_t osrs_p;
    bme280_oversampling_t osrs_h;
    bme280_standby_t standby;
    bme280_filter_t filter;
} bme280_config_t;

bool bme280_init(const bme280_config_t *config);
void bme280_read(float *temp, float *pressure, float *humidity);