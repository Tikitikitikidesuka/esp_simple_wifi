#ifndef ESP_EZ_WIFI_STA_H
#define ESP_EZ_WIFI_STA_H

#include "esp_netif.h"
#include "freertos/FreeRTOS.h"

void sta_start();
void sta_stop();

bool sta_connect(const char* ssid, const char* password);
bool sta_disconnect();

bool sta_set_connection_info(const esp_netif_ip_info_t* connection_settings);

bool sta_connected();
bool sta_connection_info(esp_netif_ip_info_t* connection_info);

#endif