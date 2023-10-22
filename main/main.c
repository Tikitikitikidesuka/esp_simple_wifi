#include "esp_ez_wifi.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main() {
  ap_start("ssid", "password", 2, 10, false);
  vTaskDelay(2000);
  ap_stop();

  sta_start();

  sta_connect("ssid", "password", 5);

  esp_netif_ip_info_t connection_info;
  if (sta_get_connection_info(&connection_info)) {
    printf("ip: " IPSTR "\n", IP2STR(&connection_info.ip));
  }
}
