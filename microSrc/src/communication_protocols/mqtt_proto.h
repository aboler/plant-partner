#ifndef MQTT_PROTO_H
#define MQTT_PROTO_H
#include "wifi.h"
#include "mqtt_client.h"
int mqtt_subscribe(const char *topic, const char *payload, int qos);
int mqtt_publish(const char *topic, const char *payload, int qos);
#endif
