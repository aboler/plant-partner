#ifndef MQTT_PROTO_H
#define MQTT_PROTO_H

#include <stdbool.h>

#define MQTT_TOPIC_ACK "plant_partner/ack"
#define MQTT_TOPIC_STARTUP "plant_partner/autocare_startup"
#define MQTT_TOPIC_AUTOCARE "plant_partner/act_tog_en"
#define MSG_SIZE    256
#define QOS         0
#define TOPIC_NAME  64
#define URI         "mqtt://172.20.10.9:1883" // CHANGE IP !!!

void mqtt_app_start(void);
bool mqtt_check_buffer_ready(void);
const char *read_data(void);
const char *read_topic(void);
int publish_mqtt(const char *topic, const char *message);
bool check_connection_ready(void);
#endif
