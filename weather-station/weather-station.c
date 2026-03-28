#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/unique_id.h"
#include "wifi.h"
#include "mqtt_client.h"
#include "sensors.h"
#include "hardware.h"
#include "air_quality.h"

#define WIFI_SSID "devolo-339"
#define WIFI_PASS "KGPQENWSYULDNCNQ"
#define BROKER_IP "192.168.178.50"
#define BROKER_PORT 1883
#define BROKER_USER "test"
#define BROKER_PASSWORD "test"

#define MQTT_WILL_TOPIC "weather/data"
#define MQTT_WILL_MSG "offline"
#define MQTT_WILL_QOS 1
#define MQTT_DEVICE_DESCRIPTION "inside"
#define MQTT_DEVICE_NAME "weather-station"

#define MOSTFET_PIN 12
#define ON_DURATION_MS 1000
#define SENSOR_WARMUP_MS (60 * 1000)

// Messintervall (z. B. 30 Minuten)
#define LOOP_DELAY_MS (30 * 60 * 1000)

int main()
{
    stdio_init_all();

    gpio_init(MOSTFET_PIN);
    gpio_set_dir(MOSTFET_PIN, true);

    hardware_init();

    static MQTT_CLIENT_DATA_T state;

    char unique_id_buf[9];
    pico_get_unique_board_id_string(unique_id_buf, sizeof(unique_id_buf));
    for (int i = 0; i < sizeof(unique_id_buf) - 1; i++) {
        unique_id_buf[i] = tolower(unique_id_buf[i]);
    }

    static char client_id_buf[32];
    snprintf(client_id_buf, sizeof(client_id_buf), "%s%s", MQTT_DEVICE_NAME, unique_id_buf);

    static char will_topic[MQTT_TOPIC_LEN];
    snprintf(will_topic, sizeof(will_topic), "%s", MQTT_WILL_TOPIC);

    while (true) {

        gpio_put(MOSTFET_PIN, 1);
        sleep_ms(ON_DURATION_MS);

        sensors_init();
        printf("Waiting %d ms for sensor warmup...\n", SENSOR_WARMUP_MS);
        sleep_ms(SENSOR_WARMUP_MS);

        SensorData data = sensors_read();
        gpio_put(MOSTFET_PIN, 0);

        AirQualityLevel level = (AirQualityLevel)data.air_quality;
        hardware_update_air_quality(level);

        if (level != AIR_GOOD) {
            hardware_tick();
        }

        if (!wifi_init_and_connect(WIFI_SSID, WIFI_PASS)) {
            printf("Wifi connect failed, retrying in 30s...\n");
            sleep_ms(30000);
            continue;
        }

        mqtt_init_client(
            &state,
            client_id_buf,
            BROKER_USER,
            BROKER_PASSWORD,
            will_topic,
            MQTT_WILL_MSG,
            MQTT_WILL_QOS,
            true
        );

        mqtt_start_client(&state, BROKER_IP, BROKER_PORT);

        while (!state.connect_done) {
            cyw43_arch_poll();
            sleep_ms(100);
        }

        char payload[256];
        snprintf(payload, sizeof(payload),
            "{\"device_id\":\"%s\","
            "\"device_description\":\"%s\","
            "\"temperature\":%.2f,"
            "\"humidity\":%.2f,"
            "\"pressure\":%.2f,"
            "\"lux\":%.2f,"
            "\"air_quality\":%.2f}",
            client_id_buf,
            MQTT_DEVICE_DESCRIPTION,
            data.temperature,
            data.humidity,
            data.pressure,
            data.lux,
            data.air_quality
        );

        printf("Publishing: %s\n", payload);
        mqtt_publish_message(&state, will_topic, payload, MQTT_WILL_QOS, false);

        sleep_ms(2000);
        mqtt_disconnect_client(&state);
        cyw43_arch_disable_sta_mode();

        printf("Cycle done, waiting...\n");
        sleep_ms(LOOP_DELAY_MS);
    }
}
