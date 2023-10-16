#include "esp_ez_wifi_sta.h"

#include <stdbool.h>
#include <string.h>

#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static const char* TAG = "wifi";

static bool initialized = false;
static bool connected = false;
static bool connecting = false;
static uint8_t max_retry_num = 0;
static uint8_t retry_num = 0;
static EventGroupHandle_t wifi_event_group;
static esp_netif_t* netif_wifi;
static esp_event_handler_instance_t instance_any_id;
static esp_event_handler_instance_t instance_got_ip;

static void init_nvs();
static void check_initialized();
static bool sta_connect_helper(const char* ssid, const char* password);
static void sta_disconnect_helper();
static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data);
static bool retry_connection();

/* --------------------------------------- */
/* --           API Functions           -- */
/* --------------------------------------- */

void sta_start() {
  if (!initialized) {
    initialized = true;

    init_nvs();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_event_group = xEventGroupCreate();
    if (wifi_event_group == NULL)
      ESP_ERROR_CHECK(ESP_ERR_NO_MEM);

    netif_wifi = esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

    ESP_LOGI(TAG, "WiFi station started.");
  } else {
    ESP_LOGI(TAG, "WiFi station is already active.");
  }
}

void sta_stop() {
  if (initialized) {
    initialized = false;

    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_deinit());
    ESP_ERROR_CHECK(esp_netif_deinit());

    vEventGroupDelete(wifi_event_group);
    esp_netif_destroy_default_wifi(netif_wifi);

    esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                          instance_any_id);
    esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                          instance_got_ip);

    ESP_ERROR_CHECK(esp_event_loop_delete_default());

    ESP_LOGI(TAG, "WiFi station stopped.");
  } else {
    ESP_LOGI(TAG, "WiFi station is not active.");
  }
}

bool sta_connect(const char* ssid, const char* password, uint8_t attempts) {
  check_initialized();

  if (connecting) {
    ESP_LOGE(TAG, "cannot connect until previous connection finishes.");
    return false;
  }

  if (connected)
    sta_disconnect_helper();

  connecting = true;
  max_retry_num = attempts - 1;
  connected = sta_connect_helper(ssid, password);
  connecting = false;

  return connected;
}

bool sta_disconnect() {
  check_initialized();

  if (connecting) {
    ESP_LOGE(TAG, "cannot disconnect while connecting.");
    return false;
  }

  sta_disconnect_helper();
  connected = false;

  return !connected;
}

bool sta_set_connection_info(const esp_netif_ip_info_t* connection_info) {
  return esp_netif_set_ip_info(netif_wifi, connection_info) == ESP_OK;
}

bool sta_connected() {
  check_initialized();

  return connected;
}

bool sta_connection_info(esp_netif_ip_info_t* sta_connection_info) {
  check_initialized();

  if (connected)
    esp_netif_get_ip_info(netif_wifi, sta_connection_info);

  return connected;
}

/* --------------------------------------- */
/* --         Helper Functions          -- */
/* --------------------------------------- */

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

static void check_initialized() {
  if (!initialized) {
    ESP_LOGE(TAG,
             "WiFi station is not active. Make sure to activate it with "
             "\'sta_start()\' before calling other WiFi station functions.");

    ESP_ERROR_CHECK(ESP_ERR_WIFI_NOT_INIT);
  }
};

static bool sta_connect_helper(const char* ssid, const char* password) {
  wifi_config_t wifi_config = {.sta = {.ssid = "", .password = ""}};
  strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
  strncpy((char*)wifi_config.sta.password, password,
          sizeof(wifi_config.sta.password) - 1);
  wifi_config.sta.ssid[sizeof(wifi_config.sta.ssid) - 1] = '\0';
  wifi_config.sta.password[sizeof(wifi_config.sta.password) - 1] = '\0';

  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  esp_wifi_connect();

  ESP_LOGI(TAG, "starting WiFi connection to SSID: %s.", wifi_config.sta.ssid);

  EventBits_t bits =
      xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                          pdTRUE, pdFALSE, portMAX_DELAY);

  bool success = false;

  if (bits & WIFI_CONNECTED_BIT) {
    ESP_LOGI(TAG, "connected to WiFi with SSID: %s.", wifi_config.sta.ssid);
    success = true;
  } else if (bits & WIFI_FAIL_BIT) {
    ESP_LOGI(TAG, "failed to connect to WiFi with SSID: %s.",
             wifi_config.sta.ssid);
    success = false;
  } else {
    ESP_LOGI(TAG, "unexpected event.");
    success = false;
  }

  return success;
}

static void sta_disconnect_helper() {
  esp_wifi_disconnect();
  ESP_LOGI(TAG, "disconnected from AP.");
}

static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data) {
  if (event_base == WIFI_EVENT) {
    if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
      if (connecting) {
        ESP_LOGI(TAG, "connection to the AP failed.");
        if (!retry_connection())
          xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
      }
    }
  } else if (event_base == IP_EVENT) {
    if (event_id == IP_EVENT_STA_GOT_IP) {
      ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
      ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
      retry_num = 0;
      xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
  }
}

static bool retry_connection() {
  if (retry_num >= max_retry_num)
    return false;

  ++retry_num;
  esp_wifi_connect();
  ESP_LOGI(TAG, "retrying to connect to the AP.");

  return true;
}