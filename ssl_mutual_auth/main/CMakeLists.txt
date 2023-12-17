
idf_component_register(SRCS "app_main.c" "app_config.c" "http_server_app.c" "sensor.c" "app_mqtt.c" "ssd1306.c" "ssd1306_i2c.c" "ssd1306_spi.c" "i2c_oled.c"
                    INCLUDE_DIRS "."
                    )
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
