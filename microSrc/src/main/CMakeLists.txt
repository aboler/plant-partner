idf_component_register(SRCS "main.c" "../peripherals/adc.c" "../peripherals/gpio_led.c" "../peripherals/pwm_pump.c"
                       PRIV_REQUIRES spi_flash
                       PRIV_REQUIRES esp_driver_gpio
                       PRIV_REQUIRES esp_adc
                       PRIV_REQUIRES esp_driver_ledc
                       PRIV_REQUIRES esp_wifi 
                       PRIV REQUIRES wpa_supplicant 
                       PRIV REQUIRES nvs_flash 
                       PRIV REQUIRES lwip 
                       PRIV REQUIRES esp_http_server
                       INCLUDE_DIRS "" "../peripherals"
                       INCLUDE_DIRS "" "../wifi"
                       )
