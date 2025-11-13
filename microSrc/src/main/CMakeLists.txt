idf_component_register(SRCS "main.c" "../peripherals/adc.c" "../peripherals/gpio_led.c" "../peripherals/pwm_pump.c" "../wifi/wifi.c"
                       PRIV_REQUIRES spi_flash
                       PRIV_REQUIRES esp_driver_gpio
                       PRIV_REQUIRES esp_adc
                       PRIV_REQUIRES esp_driver_ledc
                       PRIV_REQUIRES esp_wifi 
                       PRIV_REQUIRES wpa_supplicant 
                       PRIV_REQUIRES nvs_flash 
                       PRIV_REQUIRES lwip 
                       PRIV_REQUIRES esp_http_server
                       INCLUDE_DIRS "" "../peripherals"
                       INCLUDE_DIRS "" "../wifi"
                       )
