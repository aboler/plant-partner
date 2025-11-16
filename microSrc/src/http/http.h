
#ifndef HTTP_H
#define HTTP_H
#include "wifi.h"
void http_get(void);
esp_http_client_handle_t http_configure_handle(void);
void http_put_plant_one_shot( struct plantDataUpdate* p);
void http_put_plant_data(esp_http_client_handle_t client,struct plantDataUpdate* p);
//PUT AS AN RTOS TASK
#endif