#include "../dataTypes/plantData.h"
#include "../wifi/wifi.h"  
#define WEB_SERVER "172.20.10.2"
#define WEB_PORT "8000"
#define WEB_PUT_PATH "/plants/updatePlantByName/Sunflower"
#define JSON_BUFFER_SIZE 2048
#define IP "172.20.10.9"
static const char *TAG = "http_client";
            

static const char *JSON_TEMPLATE =
     "{\"plantName\":\"%s\","
    "\"soilMoisture\":%i,"
    "\"lightIntensity\":%i,"
    "\"nLevel\":%i,"
    "\"pLevel\":%i,"
    "\"kLevel\":%i}";



#define MAX_HTTP_OUTPUT_BUFFER 2048
static char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = {0};



static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            // Clean the buffer in case of a new request
            if (output_len == 0 && evt->user_data) {
                // we are just starting to copy the output data into the use
                memset(evt->user_data, 0, MAX_HTTP_OUTPUT_BUFFER);
            }
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                int copy_len = 0;
                if (evt->user_data) {
                    // The last byte in evt->user_data is kept for the NULL character in case of out-of-bound access.
                    copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
                    if (copy_len) {
                        memcpy(evt->user_data + output_len, evt->data, copy_len);
                    }
                } else {
                    int content_len = esp_http_client_get_content_length(evt->client);
                    if (output_buffer == NULL) {
                        // We initialize output_buffer with 0 because it is used by strlen() and similar functions therefore should be null terminated.
                        output_buffer = (char *) calloc(content_len + 1, sizeof(char));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    copy_len = MIN(evt->data_len, (content_len - output_len));
                    if (copy_len) {
                        memcpy(output_buffer + output_len, evt->data, copy_len);
                    }
                }
                output_len += copy_len;
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
#if CONFIG_EXAMPLE_ENABLE_RESPONSE_BUFFER_DUMP
                ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
#endif
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
            esp_http_client_set_header(evt->client, "From", "user@example.com");
            esp_http_client_set_header(evt->client, "Accept", "text/html");
            esp_http_client_set_redirection(evt->client);
            break;
        default:
            break;
    }
    return ESP_OK;
}




// TODO: make it so that host and path are set to some default site
esp_http_client_handle_t http_configure_handle(){
        esp_http_client_config_t config = {
        .url = "http://" IP ":8000/plants/updatePlantByName/Sunflower",
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,        // Pass address of local buffer to get response
        .disable_auto_redirect = true,
    };
    ESP_LOGI(TAG, "HTTP request with url =>");
    esp_http_client_handle_t client = esp_http_client_init(&config);
    return client;
}


static void end_http_session(esp_http_client_handle_t client){
    esp_http_client_cleanup(client);
}

void http_put_plant_one_shot(struct plantData* p){


    esp_http_client_handle_t client = http_configure_handle();


     char json_buffer[JSON_BUFFER_SIZE];

    int json_len = snprintf(json_buffer, JSON_BUFFER_SIZE,
        JSON_TEMPLATE, 
        p->plantName, 
        p->soilMoisture, 
        p->lightIntensity, 
        p->nLevel, 
        p->pLevel, 
        p->kLevel
    );

    //route to the correct website and correct formast
    esp_http_client_set_url(client, "http://" WEB_SERVER ":" WEB_PORT WEB_PUT_PATH);
    esp_http_client_set_method(client, HTTP_METHOD_PUT);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, json_buffer, json_len);
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP PUT Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP PUT request failed: %s", esp_err_to_name(err));
    }

    end_http_session(client);
}



void http_put_plant_data(esp_http_client_handle_t client, struct plantData* p){
    //initialize request template
    char json_buffer[JSON_BUFFER_SIZE];

    int json_len = snprintf(json_buffer, JSON_BUFFER_SIZE,
        JSON_TEMPLATE, 
        p->plantName, 
        p->soilMoisture, 
        p->lightIntensity, 
        p->nLevel, 
        p->pLevel, 
        p->kLevel
    );

    //route to the correct website and correct formast
    //esp_http_client_set_url(client, "http://" WEB_SERVER ":" WEB_PORT WEB_PUT_PATH);
    esp_http_client_set_method(client, HTTP_METHOD_PUT);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, json_buffer, json_len);
    
     
}  







