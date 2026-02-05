/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* MQTT (over TCP) Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include "mqtt_proto.h"

static const char *TAG = "mqtt_example";
static  esp_mqtt_client_handle_t client = NULL;
static char rx_buffer[256];
static char rx_topic[64];
static volatile bool mqtt_rx_ready;

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;

    int msg_id;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        //subscribes to all under plant_partner
        mqtt_subscribe("plant_partner/#",0);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d, return code=0x%02x ", event->msg_id, (uint8_t)*event->data);
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

    //triggered any time  a message arrives on a subscribed topic
    case MQTT_EVENT_DATA: {
        //identify current topic
        int topic_len = event->topic_len;
        if (topic_len >= sizeof(rx_topic)) {
        topic_len = sizeof(rx_topic) - 1;
        }
        //memcpy does not look for null terminator
        memcpy(rx_topic, event->topic, topic_len);
        //add null terminator to string
        rx_topic[topic_len] = '\0';


        int len = event->data_len;
        //trim off
        if (len >= sizeof(rx_buffer)) {
            len = sizeof(rx_buffer) - 1;
        }
        //copy onto rx buffer len elements from data
        memcpy(rx_buffer, event->data, len);
       rx_buffer[len] = '\0';
       mqtt_rx_ready = true;
        break;
    }

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");

        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }

        break;

    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}


//out is a buffer mqtt message is passed into
//out len is length of buffer we create
bool mqtt_poll(char *out, int out_len)
{
    if (!mqtt_rx_ready) {
        return false;
    }

    strncpy(out, rx_buffer, out_len - 1);
    out[out_len - 1] = '\0';

    mqtt_rx_ready = false;
    return true;
}

bool mqtt_poll_from(char* out, int out_len, const char* topic)
{
    if (!mqtt_rx_ready) {
        return false;
    }
    
    //check origin/topic of message
    if (strcmp(rx_topic, topic) == 0) {
        // Topic matches - copy the message
        strncpy(out, rx_buffer, out_len - 1);
        out[out_len - 1] = '\0';
        
        mqtt_rx_ready = false;
        return true;
    }
    
    return false;
}



static void mqtt_app_start(void)
{   
    //use your computer's ip
    //using Mosquitto
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://192.168.1.242:1883"
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

static void mqtt_init(void){
    mqtt_app_start();
}


bool mqtt_connect(void){
    mqtt_init();
    
    const int timeout_ms = 30000;
    const int poll_interval_ms = 100;
    int elapsed_ms = 0;
    
    // Wait for acknowledgment with timeout
    while(elapsed_ms < timeout_ms) {
        if(mqtt_poll_from(rx_buffer, 256, "plant_partner/ack")) {
            ESP_LOGI(TAG, "MQTT connection acknowledged");
            return true;
        }
        
        // Publish ack request
        mqtt_publish("plant_partner/ack", "ack", 0);
        
        // Wait before next poll
        vTaskDelay(pdMS_TO_TICKS(poll_interval_ms));
        elapsed_ms += poll_interval_ms;
    }
    
    // Timeout occurred
    ESP_LOGE(TAG, "MQTT connection timeout - no acknowledgment received");
    return false;
}

int mqtt_publish(const char *topic, const char *payload, int qos)
{
    if (client == NULL) {
        ESP_LOGE(TAG, "MQTT client not initialized");
        return -1;
    }

    return esp_mqtt_client_publish(
        client,
        topic,
        payload,
        0,      
        qos,
        0       
    );
}

int mqtt_subscribe(const char *topic, int qos)
{
    if (client == NULL) {
        ESP_LOGE(TAG, "MQTT client not initialized");
        return -1;
    }

    return esp_mqtt_client_subscribe(
        client,
        topic,
        qos
    );
}

