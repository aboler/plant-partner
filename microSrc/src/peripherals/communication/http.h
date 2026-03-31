#ifndef HTTP_H
#define HTTP_H

#include "wifi.h"
#include "../dataTypes/plantData.h"

#define WEB_SERVER              "172.20.10.2"
#define WEB_PORT                "8000"
#define WEB_PUT_PATH            "/plants/updatePlantByName/Sunflower"
#define JSON_BUFFER_SIZE        2048
#define MAX_HTTP_OUTPUT_BUFFER  2048
#define IP                      "172.20.10.8" // CHANGE IP !!!

void http_get(void);
esp_http_client_handle_t http_configure_handle(void);
void http_put_plant_one_shot(struct plantData* p);
void http_put_plant_data(esp_http_client_handle_t client, struct plantData* p);

#endif