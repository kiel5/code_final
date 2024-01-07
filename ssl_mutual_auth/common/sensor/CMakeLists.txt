set(app_src sensor.c)
set(pri_req driver input_iot app_mqtt oled esp_adc)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    REQUIRES ${pri_req})

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")