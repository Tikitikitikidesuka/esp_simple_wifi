#include "esp_ez_wifi_sta.h"

#include <stdbool.h>
#include <string.h>

#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#define CONNECT_MAXIMUM_RETRY 5

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static bool initialized = false;
static bool connected = false;
static uint8_t retry_num = 0;
static esp_netif_ip_info_t connection_info;
static EventGroupHandle_t wifi_event_group;

static void init_nvs();
static void start_station();
static void stop_station();
static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data);

/* --------------------------------------- */
/* --           API Functions           -- */
/* --------------------------------------- */

void sta_init() {
  if (!initialized) {
    init_nvs();
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
  } else {
    ESP_LOGI(TAG, "station already initialized.");
  }
}

void sta_connect(char* ssid, char* password) {
  if (!initialized) {
    ESP_LOGI(TAG, "initialize station with function \'init_station()\'.");
    sta_init();
    initialized = true;
  }

  start_station(ssid, password);
}

void sta_disconnect() {
  stop_station();
}

bool sta_connected() {
  return connected;
}

bool sta_connectionInfo(esp_netif_ip_info_t* sta_connection_info) {
  //memcpy(sta_connection_info, )
  *sta_connection_info = connection_info;
  return connected;
}

/* --------------------------------------- */
/* --         Helper Functions          -- */
/* --------------------------------------- */

static void start_station(char* ssid, char* password) {
  wifi_event_group = xEventGroupCreate();

  ESP_ERROR_CHECK(esp_netif_init());

  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

  wifi_config_t wifi_config = {.sta = {.ssid = "", .password = ""}};
  strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
  strncpy((char*)wifi_config.sta.password, password,
          sizeof(wifi_config.sta.password) - 1);
  wifi_config.sta.ssid[sizeof(wifi_config.sta.ssid) - 1] = '\0';
  wifi_config.sta.password[sizeof(wifi_config.sta.password) - 1] = '\0';

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAG, "init_station finished.");

  EventBits_t bits =
      xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                          pdFALSE, pdFALSE, portMAX_DELAY);

  if (bits & WIFI_CONNECTED_BIT) {
    ESP_LOGI(TAG, "connected to ap SSID: %s.", ssid);
    connected = true;
  } else if (bits & WIFI_FAIL_BIT) {
    ESP_LOGI(TAG, "failed to connect to SSID: %s.", ssid);
    connected = false;
  } else {
    ESP_LOGI(TAG, "unexpected event.");
    connected = false;
  }
}

static void stop_station() {
  esp_wifi_stop();
  connected = false;
}

static void init_nvs() {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_LOGI(TAG, "init_nvs finished.");
}

static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data) {
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED) {
    ESP_LOGI(TAG, "connect to the AP fail");
    if (retry_num < CONNECT_MAXIMUM_RETRY) {
      esp_wifi_connect();
      retry_num++;
      ESP_LOGI(TAG, "retry to connect to the AP");
    } else {
      xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
    }
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
    ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
    connection_info = event->ip_info;
    retry_num = 0;
    xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
  }
}