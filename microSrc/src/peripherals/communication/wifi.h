#ifndef WIFI_H
#define WIFI_H
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_server.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include <sys/param.h>
#include <stdlib.h>
#include <ctype.h>
#include "esp_system.h"
#include "esp_http_client.h"
#include "esp_tls.h"

//MQTT additions
#include "mqtt_client.h"


// CHANGE WIFI !!!
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SSID               "Aspen"
#define ESP_WIFI_PASS               "esp32wifi"
#define EXAMPLE_ESP_MAXIMUM_RETRY   4
#define WIFI_CONNECTED_BIT          BIT0
#define WIFI_FAIL_BIT               BIT1

void start_wifi();

#endif