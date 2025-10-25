idf_component_register(
                        SRCS "main.c"
                        PRIV_REQUIRES spi_flash
                        PRIV_REQUIRES esp_adc
                        PRIV_REQUIRES esp_driver_gpio
                        INCLUDE_DIRS " ")
