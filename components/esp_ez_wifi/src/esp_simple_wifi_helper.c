#include "esp_simple_wifi_helper.h"

#include <inttypes.h>
#include <string.h>

#include "esp_simple_wifi_constants.h"

wifi_credential_status_t wifi_credential_validation(const char* ssid,
                                                    const char* password) {
  uint8_t ssid_length = strnlen(ssid, MAX_WIFI_SSID_LENGTH + 1);
  uint8_t password_length = strnlen(password, MAX_WIFI_PASSWORD_LENGTH + 1);

  if (ssid_length < MIN_WIFI_SSID_LENGTH)
    return WIFI_CRED_SSID_SHORT;
  if (ssid_length > MAX_WIFI_SSID_LENGTH)
    return WIFI_CRED_SSID_LONG;

  if (password_length < MIN_WIFI_PASSWORD_LENGTH)
    return WIFI_CRED_PASSWORD_SHORT;
  if (password_length > MAX_WIFI_PASSWORD_LENGTH)
    return WIFI_CRED_PASSWORD_LONG;

  return WIFI_CRED_OK;
}