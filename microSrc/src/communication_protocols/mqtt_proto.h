#ifndef MQTT_PROTO_H
#define MQTT_PROTO_H
#include "wifi.h"
#include "mqtt_client.h"

#define MQTT_TOPICS "plant_partner/#"
#define QOS 0
#define URI "mqtt://192.168.1.242:1883" // CHANGE IP !!!

void mqtt_app_start(void);
bool mqtt_check_buffer_ready(void);
const char *read_data(void);
const char *read_topic(void);

#endif
