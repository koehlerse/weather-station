#ifndef MQ135_H
#define MQ135_H

#include <stdint.h>
#include "air_quality.h"

void mq135_init(uint adc_gpio, uint adc_channel);
uint16_t mq135_read_raw(void);
float mq135_read_voltage_adcnode(void);
float mq135_read_voltage_sensor(void);
AirQualityLevel mq135_get_air_quality(void);

#endif