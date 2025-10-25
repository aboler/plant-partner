idf_component_register(SRCS "oneshot_read_main.c"
                    PRIV_REQUIRES esp_adc
                    REQUIRES esp_driver_gpio
                    INCLUDE_DIRS ".")
