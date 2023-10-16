#include "esp_ez_wifi_sta.h"

#include "freertos/FreeRTOS.h"

void app_main() {
  sta_start();

  sta_connect("ssid", "password", 5);

  esp_netif_ip_info_t connection_info;
  if (sta_get_connection_info(&connection_info)) {
    printf("ip: " IPSTR "\n", IP2STR(&connection_info.ip));
  }
}
