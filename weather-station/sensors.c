#include <pico/stdlib.h>
#include "hardware/i2c.h"
#include "sensors.h"
#include "bme280.h"
#include "mq135.h"
#include "bh1750.h"
#include <stdio.h>
#include "hardware/pwm.h"


#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5
#define BME280_ADDR 0x76
#define BH1750_ADDR 0x23
#define MQ135_ADC_GPIO 26
#define MQ135_ADC_CHANNEL 0

static bme280_config_t bme_cfg;

void sensors_init(void) {
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    bi_decl(bi_2pins_with_func(I2C_SDA, I2C_SCL, GPIO_FUNC_I2C));

    bme_cfg = (bme280_config_t) {
        .i2c = I2C_PORT,
        .address = BME280_ADDR,
        .mode = BME280_MODE_NORMAL,
        .osrs_t = BME280_OVERSAMPLING_16X,
        .osrs_p = BME280_OVERSAMPLING_16X,
        .osrs_h = BME280_OVERSAMPLING_16X,
        .standby = BME280_STANDBY_500_MS,
        .filter = BME280_FILTER_16
    };

    if (!bme280_init(&bme_cfg)) {
        panic("BME280 not found!\n");
    } else {
        printf("BME280 initialized\n");
    }

    bh1750_write(BH1750_POWER_ON);
    sleep_ms(200);
    bh1750_write(BH1750_CONTINUOUS_HIGH_RES_MODE);
    printf("BH1750 initialized\n");

    mq135_init(MQ135_ADC_GPIO, MQ135_ADC_CHANNEL);
    printf("MQ135 initialized on GPIO%d (ADC%d)\n", MQ135_ADC_GPIO, MQ135_ADC_CHANNEL);
}

SensorData sensors_read(void) {
    SensorData data = {0};
    float temp, press, hum;
    bme280_read(&temp, &press, &hum);

    data.temperature = temp;
    data.humidity = hum;
    data.pressure = press;

    data.lux = bh1750_read_lux();

    AirQualityLevel level = mq135_get_air_quality();
    data.air_quality = (float)level; // 0 = good; 1 = medium; 2 = bad

    return data;
}