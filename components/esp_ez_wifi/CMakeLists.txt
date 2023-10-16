set(component_srcs)

idf_component_register(SRCS "esp_ez_wifi_sta.c" "esp_ez_wifi_ap.c" "${component_srcs}"
                       PRIV_REQUIRES driver
                       INCLUDE_DIRS "."
                       REQUIRES
                            esp_wifi
                            nvs_flash)
