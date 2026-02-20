#ifndef WIFI_H
#define WIFI_H

#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_tls.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include <sys/param.h>

#define DEBUG_MODE_WIFI


#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SSID               "SpectrumSetup-84"         // CHANGE WIFI !!!
#define ESP_WIFI_PASS               "lightsquirrel484"
#define EXAMPLE_ESP_MAXIMUM_RETRY   4
#define WIFI_CONNECTED_BIT          BIT0
#define WIFI_FAIL_BIT               BIT1

void start_wifi();

#endif