#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "mq135.h"

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

float mq135_read_voltage(void) {
    uint16_t raw = mq135_read_raw();
    return raw * 3.3f / (1 << 12);
}

AirQualityLevel mq135_get_air_quality(void) {
    float voltage = mq135_read_voltage();
    if (voltage < 1.0f) {
        return AIR_GOOD;
    } else if (voltage < 2.0f) {
        return AIR_MEDIUM;
    } else {
        return AIR_BAD;
    }
}