#ifndef ESP_EZ_WIFI_AP_H
#define ESP_EZ_WIFI_AP_H

#include <inttypes.h>
#include <stdbool.h>

#include "esp_ez_wifi_helper.h"

typedef enum wifi_ap_start_status_t {
  WIFI_AP_START_SUCCESS,
  WIFI_AP_START_INVALID_CREDENTIALS,
  WIFI_AP_START_ALREADY_ACTIVE,
} wifi_ap_start_status_t;

wifi_ap_start_status_t ap_start(const char* ssid, const char* password,
                                int8_t channel, int8_t max_connections,
                                bool hidden);
void ap_stop();

#endif