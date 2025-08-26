#include <stdio.h>
#include <string.h>
#include <pico/cyw43_arch.h>
#include "mqtt_client.h"

#define MQTT_KEEP_ALIVE_S 60

static void pub_request_cb(__unused void *arg, err_t err) {
    if (err != 0) {
        printf("Publish failed: %d\n", err);
    } else {
        printf("Publish OK\n");
    }
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    MQTT_CLIENT_DATA_T* state = (MQTT_CLIENT_DATA_T*)arg;

    if (status == MQTT_CONNECT_ACCEPTED) {
        state->connect_done = true;
    } else if (status == MQTT_CONNECT_DISCONNECTED) {
        if (!state->connect_done) {
            panic("Failed to connect to mqtt server");
        }
    } else {
        panic("Unexpected status");
    }
}

bool mqtt_publish_message(MQTT_CLIENT_DATA_T *state, const char *topic, const char *msg, int qos, bool retain) {
    if (!mqtt_client_is_connected(state->mqtt_client_inst)) {
        printf("MQTT not connected, cannot publish\n");
        return false;
    }
    err_t err = mqtt_publish(state->mqtt_client_inst, topic, msg, strlen(msg), qos, retain, pub_request_cb, state);
    return (err == ERR_OK);
}

void mqtt_disconnect_client(MQTT_CLIENT_DATA_T *state) {
    if (mqtt_client_is_connected(state->mqtt_client_inst)) {
        printf("Disconnecting MQTT client...\n");
        mqtt_disconnect(state->mqtt_client_inst);
    }
    state->connect_done = false;
}

void mqtt_init_client(MQTT_CLIENT_DATA_T *state, const char *client_id, const char *user, const char *pass, const char *will_topic, const char *will_msg, int will_qos, bool will_retain) {
    memset(state, 0, sizeof(MQTT_CLIENT_DATA_T));
    state->mqtt_client_info.client_id = client_id;
    state->mqtt_client_info.keep_alive = MQTT_KEEP_ALIVE_S;
    state->mqtt_client_info.client_user = user;
    state->mqtt_client_info.client_pass = pass;
    state->mqtt_client_info.will_topic = will_topic;
    state->mqtt_client_info.will_msg = will_msg;
    state->mqtt_client_info.will_qos = will_qos;
    state->mqtt_client_info.will_retain = will_retain;
}

void mqtt_start_client(MQTT_CLIENT_DATA_T *state, const char *ip, int port) {
    if (!ip4addr_aton(ip, &state->mqtt_server_address)) {
        panic("Invalid IP address\n");
    }

    state->mqtt_client_inst = mqtt_client_new();
    if (!state->mqtt_client_inst) {
        panic("MQTT client instance creation error");
    }

    printf("Connecting to MQTT server at %s\n", ipaddr_ntoa(&state->mqtt_server_address));

    cyw43_arch_lwip_begin();
    if (mqtt_client_connect(state->mqtt_client_inst, &state->mqtt_server_address, port, mqtt_connection_cb, state, &state->mqtt_client_info) != ERR_OK) {
        panic("MQTT broker connection error");
    }
    cyw43_arch_lwip_end();
}