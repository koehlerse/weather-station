#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>
#include <stdbool.h>
#include "air_quality.h"

#define LED_PIN 15
#define BUZZER_PIN 14

void hardware_init(void);
void hardware_update_air_quality(AirQualityLevel level);
void hardware_tick(void);

#endif