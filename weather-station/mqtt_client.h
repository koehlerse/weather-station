#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include "lwip/apps/mqtt.h"
#include "lwip/ip_addr.h"
#include <stdbool.h>

#ifndef MQTT_TOPIC_LEN
#define MQTT_TOPIC_LEN 100
#endif

typedef struct {
    mqtt_client_t* mqtt_client_inst;
    struct mqtt_connect_client_info_t mqtt_client_info;
    char data[MQTT_OUTPUT_RINGBUF_SIZE];
    char topic[MQTT_TOPIC_LEN];
    uint32_t len;
    ip_addr_t mqtt_server_address;
    bool connect_done;
    int subscribe_count;
    bool stop_client;
} MQTT_CLIENT_DATA_T;

void mqtt_init_client(MQTT_CLIENT_DATA_T *state, const char *client_id, 
    const char *user, const char *pass, 
    const char *will_topic, const char *will_msg, 
    int will_qos, bool will_retain);

void mqtt_start_client(MQTT_CLIENT_DATA_T *state, const char *ip, int port);

void mqtt_disconnect_client(MQTT_CLIENT_DATA_T *state);

bool mqtt_publish_message(MQTT_CLIENT_DATA_T *state, const char *topic, const char *msg, int qos, bool retain);

#endif