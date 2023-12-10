#ifndef ESP_EZ_WIFI_HELPER_H
#define ESP_EZ_WIFI_HELPER_H

typedef enum wifi_credential_status_t {
    WIFI_CRED_OK,
    WIFI_CRED_SSID_SHORT,
    WIFI_CRED_PASSWORD_SHORT,
    WIFI_CRED_SSID_LONG,
    WIFI_CRED_PASSWORD_LONG
} wifi_credential_status_t;

wifi_credential_status_t wifi_credential_validation(const char* ssid, const char* password);

#endif