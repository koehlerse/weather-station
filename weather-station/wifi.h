#ifndef WIFI_H
#define WIFI_H

#include <stdbool.h>

bool wifi_init_and_connect(const char *ssid, const char *pass);

#endif