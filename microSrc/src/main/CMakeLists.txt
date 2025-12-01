idf_component_register(
    SRCS 
        "main.c" 
        "../peripherals/adc.c" 
        "../peripherals/gpio.c" 
        "../peripherals/pwm_pump.c"
        "../peripherals/uart_driver.c"
        "../wifi/wifi.c" 
        "../http/http.c"

    PRIV_REQUIRES 
        spi_flash
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
        esp_driver_uart
        

    INCLUDE_DIRS 
        "../peripherals"
        "../wifi"
        "../http"
)
