cmake_minimum_required(VERSION 3.12)

set(srcs
	src/esp_simple_wifi_sta.c
	src/esp_simple_wifi_ap.c
	src/esp_simple_wifi_helper.c
)

set(private_dependencies
     esp_wifi
     esp_netif
     nvs_flash
     driver
)

idf_component_register(
	SRCS ${srcs}
	INCLUDE_DIRS "include"
     PRIV_INCLUDE_DIRS "src"
     PRIV_REQUIRES ${private_dependencies}
)

project(EspSimpleWifi VERSION 0.0.0)
