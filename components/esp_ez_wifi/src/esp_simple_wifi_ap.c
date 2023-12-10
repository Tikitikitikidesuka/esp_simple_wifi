#include "esp_simple_wifi_ap.h"

#include <string.h>

#include "esp_log.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include "esp_simple_wifi_constants.h"
#include "esp_simple_wifi_helper.h"

static const char* TAG = "wifi_ap";

static bool initialized = false;
static esp_netif_t* netif_wifi;
static esp_event_handler_instance_t instance_any_id;

static void init_nvs();
static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data);

/* --------------------------------------- */
/* --           API Functions           -- */
/* --------------------------------------- */

wifi_ap_start_status_t ap_start(const char* ssid, const char* password,
                                int8_t channel, int8_t max_connections,
                                bool hidden) {
  if (initialized) {
    ESP_LOGI(TAG, "WiFi access point is already active.");
    return WIFI_AP_START_ALREADY_ACTIVE;
  }

  if (wifi_credential_validation(ssid, password) != WIFI_CRED_OK)
    return WIFI_AP_START_INVALID_CREDENTIALS;

  initialized = true;

  init_nvs();
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  netif_wifi = esp_netif_create_default_wifi_ap();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));

  wifi_config_t wifi_config = {
      .ap =
          {
              .ssid = "",
              .ssid_len = 0,
              .channel = channel,
              .ssid_hidden = hidden,
              .password = "",
              .max_connection = max_connections,
              .authmode = WIFI_AUTH_WPA3_PSK,
              .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
              .pmf_cfg =
                  {
                      .required = true,
                  },
          },
  };

  strncpy((char*)wifi_config.ap.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);

  if (strnlen(password, MAX_WIFI_PASSWORD_LENGTH + 1) == 0) {
    wifi_config.ap.authmode = WIFI_AUTH_OPEN;
  } else {
    strncpy((char*)wifi_config.ap.password, password,
            sizeof(wifi_config.sta.password) - 1);
  }

  wifi_config.ap.ssid[sizeof(wifi_config.ap.ssid) - 1] = '\0';
  wifi_config.ap.password[sizeof(wifi_config.ap.password) - 1] = '\0';

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAG, "WiFi access point started.");
  return WIFI_AP_START_SUCCESS;
}

void ap_stop() {
  if (initialized) {
    initialized = false;

    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_deinit());

    esp_netif_destroy_default_wifi(netif_wifi);
    esp_netif_deinit();  // No error check since it is still not supported

    esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                          instance_any_id);

    ESP_ERROR_CHECK(esp_event_loop_delete_default());

    ESP_LOGI(TAG, "WiFi access point stopped.");
  } else {
    ESP_LOGI(TAG, "WiFi access point is not active.");
  }
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

static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data) {
  if (event_base == WIFI_EVENT) {
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
      wifi_event_ap_staconnected_t* event =
          (wifi_event_ap_staconnected_t*)event_data;
      ESP_LOGI(TAG, "station " MACSTR " joined, AID=%d", MAC2STR(event->mac),
               event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
      wifi_event_ap_stadisconnected_t* event =
          (wifi_event_ap_stadisconnected_t*)event_data;
      ESP_LOGI(TAG, "station " MACSTR " left, AID=%d", MAC2STR(event->mac),
               event->aid);
    }
  }
}
