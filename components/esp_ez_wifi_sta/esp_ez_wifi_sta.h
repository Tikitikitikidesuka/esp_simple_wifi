#ifndef ESP_EZ_WIFI_STA_H
#define ESP_EZ_WIFI_STA_H

#include "esp_netif.h"
#include "freertos/FreeRTOS.h"

static const char* TAG = "wifi";

void sta_init();

void sta_connect(char* ssid, char* password);
void sta_disconnect();

bool sta_connected();
bool sta_connectionInfo(esp_netif_ip_info_t* connection_info);

#endif