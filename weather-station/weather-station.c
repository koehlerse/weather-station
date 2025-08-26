#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/unique_id.h"
#include "wifi.h"
#include "mq135.h"
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
#define MQTT_DEVICE_NAME "pico"

#define LED_PIN 15
#define BUZZER_BIN

int main()
{
    stdio_init_all();

    static MQTT_CLIENT_DATA_T state;


    char unique_id_buf[5];
    pico_get_unique_board_id_string(unique_id_buf, sizeof(unique_id_buf));
    for (int i = 0; i < sizeof(unique_id_buf) - 1; i++) {
        unique_id_buf[i] = tolower(unique_id_buf[i]);
    }

    static char client_id_buf[32];
    snprintf(client_id_buf, sizeof(client_id_buf), "%s%s", MQTT_DEVICE_NAME, unique_id_buf);

    static char will_topic[MQTT_TOPIC_LEN];
    sniprintf(will_topic, sizeof(will_topic), "%s", MQTT_WILL_TOPIC);

    sensors_init();
    hardware_init();
    printf("Waiting 5 mins for MQ135\n");
    sleep_ms(5 * 60 * 1000); // 5min


    while (true) {

        SensorData data = sensors_read();
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

        mqtt_init_client(&state, client_id_buf, BROKER_USER, BROKER_PASSWORD, will_topic, MQTT_WILL_MSG, MQTT_WILL_QOS, true);

        mqtt_start_client(&state, BROKER_IP, BROKER_PORT);

        while (!state.connect_done) {
            cyw43_arch_poll();
            sleep_ms(100);
        }

        char payload[256];
        snprintf(payload, sizeof(payload), 
            "{\"device_id\":\"%s\","
            "\"temperature\":%.2f,"
            "\"humidity\":%.2f,"
            "\"pressure\":%.2f,"
            "\"lux\":%.2f,"
            "\"air_quality\":%.2f}",
            client_id_buf, 
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

        if (level != AIR_GOOD) {
            panic("Bad air quality, sleeping 1 min...\n");
            sleep_ms(1 * 60 * 1000); // 1min
        } else {
            printf("Sleeping for 30min...\n");
            sleep_ms(30 * 60 * 1000); // 30min
        }
    }
}
