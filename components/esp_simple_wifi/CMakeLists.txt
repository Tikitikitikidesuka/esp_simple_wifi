cmake_minimum_required(VERSION 3.12)

set(srcs
	src/esp_simple_wifi_sta.c
	src/esp_simple_wifi_ap.c
	src/esp_simple_wifi_helper.c
)

set(public_dependencies
     esp_netif
)

set(private_dependencies
     esp_wifi
     nvs_flash
     driver
)

idf_component_register(
	SRCS ${srcs}
	INCLUDE_DIRS "include"
     PRIV_INCLUDE_DIRS "src"
     REQUIRES ${public_dependencies}
     PRIV_REQUIRES ${private_dependencies}
)

project(EspSimpleWifi VERSION 0.0.0)
