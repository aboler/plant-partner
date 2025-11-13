idf_component_register(SRCS "main.c" "../peripherals/adc.c" "../peripherals/gpio.c" "../peripherals/pwm_pump.c" "../peripherals/uart_driver.c"
                       PRIV_REQUIRES spi_flash
                       PRIV_REQUIRES esp_driver_gpio
                       PRIV_REQUIRES esp_adc
                       PRIV_REQUIRES esp_driver_ledc
                       PRIV_REQUIRES esp_driver_uart
                       INCLUDE_DIRS "" "../peripherals")
