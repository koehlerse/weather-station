#include "pico/stdlib.h"
#include "hardware.h"

static AirQualityLevel current_level = AIR_GOOD;
static uint32_t last_toggle_ms = 0;
static bool led_state = false;
static bool buzzer_state = false;

#define BLINK_INTERVAL 500

void hardware_init(void) {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, true);

    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, true);

    gpio_put(LED_PIN, 0);
    gpio_put(BUZZER_PIN, 0);
}

void hardware_update_air_quality(AirQualityLevel level) {
    current_level = level;
    if (level == AIR_GOOD) {
        led_state = false;
        buzzer_state = false;
        gpio_put(LED_PIN, 0);
        gpio_put(BUZZER_PIN, 0);
    }
}

void hardware_tick(void) {
    if (current_level == AIR_GOOD) return;

    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (now - last_toggle_ms >= BLINK_INTERVAL) {
        last_toggle_ms = now;

        led_state = !led_state;
        gpio_put(LED_PIN, led_state);

        if (current_level == AIR_BAD) {
            buzzer_state = !buzzer_state;
            gpio_put(BUZZER_PIN, buzzer_state);
        }
    }
}