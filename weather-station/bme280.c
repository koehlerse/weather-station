#include "bme280.h"

#include <math.h>
#include "pico/stdlib.h"

static i2c_inst_t *i2c_port;
static uint8_t i2c_addr;

static uint16_t dig_T1;
static int16_t dig_T2, dig_T3;
static uint16_t dig_P1;
static int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
static uint8_t dig_H1, dig_H3;
static int16_t dig_H2, dig_H4, dig_H5, dig_H6;

static int32_t t_fine;

static void write_reg(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = {reg, val};
    i2c_write_blocking(i2c_port, i2c_addr, buf, 2, false);
}

static void read_reg(uint8_t reg, uint8_t *buf, size_t len)
{
    i2c_write_blocking(i2c_port, i2c_addr, &reg, 1, true);
    i2c_read_blocking(i2c_port, i2c_addr, buf, len, false);
}

static uint16_t read16_LE(uint8_t reg)
{
    uint8_t buf[2];
    read_reg(reg, buf, 2);
    return (uint16_t)(buf[0] | (buf[1] << 8));
}

static int16_t readS16_LE(uint8_t reg)
{
    return (int16_t)read16_LE(reg);
}

bool bme280_init(const bme280_config_t *config)
{
    i2c_port = config->i2c;
    i2c_addr = config->address;

    uint8_t id;
    read_reg(0xD0, &id, 1);
    if (id != 0x60)
    {   
        return false;
    }

    write_reg(0xE0, 0xB6); // soft reset
    sleep_ms(300);

    // read compensation parameters
    dig_T1 = read16_LE(0x88);
    dig_T2 = readS16_LE(0x8A);
    dig_T3 = readS16_LE(0x8C);
    dig_P1 = read16_LE(0x8E);
    dig_P2 = readS16_LE(0x90);
    dig_P3 = readS16_LE(0x92);
    dig_P4 = readS16_LE(0x94);
    dig_P5 = readS16_LE(0x96);
    dig_P6 = readS16_LE(0x98);
    dig_P7 = readS16_LE(0x9A);
    dig_P8 = readS16_LE(0x9C);
    dig_P9 = readS16_LE(0x9E);
    dig_H1 = 0;
    read_reg(0xA1, &dig_H1, 1);
    dig_H2 = readS16_LE(0xE1);
    dig_H3 = 0;
    read_reg(0xE3, &dig_H3, 1);
    uint8_t e4, e5, e6;
    read_reg(0xE4, &e4, 1);
    read_reg(0xE5, &e5, 1);
    read_reg(0xE6, &e6, 1);
    dig_H4 = (e4 << 4) | (e5 & 0x0F);
    dig_H5 = (e6 << 4) | (e5 >> 4);
    read_reg(0xE7, (uint8_t *)&dig_H6, 1);

    // set sensor config
    write_reg(0xF2, config->osrs_h);

    uint8_t ctrl_meas = (config->osrs_t << 5) | (config->osrs_p << 2) | config->mode;
    write_reg(0xF4, ctrl_meas);

    uint8_t config_reg = (config->standby << 5) | (config->filter << 2);
    write_reg(0xF5, config_reg);
    return true;
}

void bme280_read(float* temperature, float* pressure, float* humidity)
{
    uint8_t data[8];
    read_reg(0xF7, data, 8);

    int32_t adc_P = (int32_t)(data[0] << 12 | data[1] << 4 | (data[2] >> 4));
    int32_t adc_T = (int32_t)(data[3] << 12 | data[4] << 4 | (data[5] >> 4));
    int32_t adc_H = (int32_t)(data[6] << 8 | data[7]);

    // temp compensation
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    *temperature = (float)((t_fine * 5 + 128) >> 8) / 100.0;

    // pressure compensation
    int64_t var1_p = ((int64_t)t_fine) - 128000;
    int64_t var2_p = var1_p * var1_p * (int64_t)dig_P6;
    var2_p = var2_p + ((var1_p * (int64_t)dig_P5) << 17);
    var2_p = var2_p + (((int64_t)dig_P4) << 35);
    var1_p = ((var1_p * var1_p * (int64_t)dig_P3) >> 8) + ((var1_p * (int64_t)dig_P2) << 12);
    var1_p = (((((int64_t)1) << 47) + var1_p)) * ((int64_t)dig_P1) >> 33;
    if (var1_p == 0)
    {
        *pressure = 0; // avoid div by zero
    }
    else
    {
        int64_t p = 1048576 - adc_P;
        p = (((p << 31) - var2_p) * 3125) / var1_p;
        var1_p = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
        var2_p = (((int64_t)dig_P8) * p) >> 19;
        p = ((p + var1_p + var2_p) >> 8) + (((int64_t)dig_P7) << 4);
        *pressure = (float)p / 256.0 / 100.0; // to hPa
    }

    // humidity compensation
    int32_t v_x1_u32r = (t_fine - ((int32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) -
                    (((int32_t)dig_H5) * v_x1_u32r)) +
                   16384) >>
                  15) *
                 (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) *
                      (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) + 32768)) >>
                     10) +
                    2097152) *
                       ((int32_t)dig_H2) +
                   8192) >>
                  14));
    v_x1_u32r = v_x1_u32r -
                (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                  ((int32_t)dig_H1)) >>
                 4);
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    *humidity = (float)(v_x1_u32r >> 12) / 1024.0;
}