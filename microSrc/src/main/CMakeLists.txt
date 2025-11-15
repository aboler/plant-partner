idf_component_register(
    SRCS 
        "main.c" 
        "../peripherals/adc.c" 
        "../peripherals/gpio_led.c" 
        "../peripherals/pwm_pump.c" 
        "../wifi/wifi.c" 
        "../http/http.c"

    PRIV_REQUIRES 
        esp_driver_gpio
        esp_adc
        esp_driver_ledc
        esp_wifi 
        esp_http_server
        nvs_flash
        freertos 
        lwip
        esp_system
        esp_http_client
        esp_event
        esp-tls
        

    INCLUDE_DIRS 
        "../peripherals"
        "../wifi"
        "../http"
)