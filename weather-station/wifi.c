#include <stdio.h>
#include <pico/cyw43_arch.h>
#include "wifi.h"

const uint32_t timeout = 30000; // 30 sec

bool wifi_init_and_connect(const char *ssid, const char *pass) {
    if (cyw43_arch_init()) {
        printf("Wi-Fi Init failed\n");
        return false;
    }

    cyw43_arch_enable_sta_mode();

    printf("Connect with WLan: %s\n", ssid);
    sleep_ms(1000);
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, timeout)) {
        printf("Wi-Fi connection failed\n");
        cyw43_arch_deinit();
        return false;
    }

    printf("Wi-Fi connection success\n");
    return true;
}