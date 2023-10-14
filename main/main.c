#include "esp_ez_wifi_sta.h"

#include "freertos/FreeRTOS.h"

void app_main() {
  sta_connect("ssid", "password");

  esp_netif_ip_info_t connection_info;
  if (sta_connectionInfo(&connection_info)) {
    printf("ip: " IPSTR, IP2STR(&connection_info.ip));
  }
}