#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "mq135.h"

#define MQ135_DIV_RTOP 10000.0f // 10k
#define MQ135_DIV_RBOTTOM 5100.f // 5.1k
#define MQ135_DIV_GAIN (MQ135_DIV_RBOTTOM / (MQ135_DIV_RTOP + MQ135_DIV_RBOTTOM))

static uint _adc_channel;

void mq135_init(uint adc_gpio, uint adc_channel) {
    adc_init();
    adc_gpio_init(adc_gpio);
    _adc_channel = adc_channel;
}

uint16_t mq135_read_raw(void) {
    adc_select_input(_adc_channel);
    return adc_read();
}

float mq135_read_voltage_adcnode(void) {
    uint16_t raw = mq135_read_raw(); // 0..4095
    return raw * 3.3f / 4096.0f ;
}

float mq135_read_voltage_sensor(void) {
    float v_adc = mq135_read_voltage_adcnode();
    return v_adc / MQ135_DIV_GAIN;
}

AirQualityLevel mq135_get_air_quality(void) {
    float voltage = mq135_read_voltage_sensor();
    if (voltage < 1.0f) {
        return AIR_GOOD;
    } else if (voltage < 2.0f) {
        return AIR_MEDIUM;
    } else {
        return AIR_BAD;
    }
}