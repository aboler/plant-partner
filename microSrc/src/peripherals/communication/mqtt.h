#ifndef MQTT_PROTO_H
#define MQTT_PROTO_H

#define MQTT_TOPICS "plant_partner/#"
#define MSG_SIZE    256
#define QOS         0
#define TOPIC_NAME  64
#define URI         "mqtt://172.20.10.8:1883" // CHANGE IP !!!

void mqtt_app_start(void);
bool mqtt_check_buffer_ready(void);
const char *read_data(void);
const char *read_topic(void);

#endif
