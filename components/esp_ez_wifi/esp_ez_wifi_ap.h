#ifndef ESP_EZ_WIFI_AP_H
#define ESP_EZ_WIFI_AP_H

#include <inttypes.h>
#include <stdbool.h>

void ap_start(const char* ssid, const char* password, int8_t channel,
              int8_t max_connections, bool hidden);
void ap_stop();

#endif