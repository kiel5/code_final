set(app_src app_mqtt.c)
set(pri_req driver esp_timer esp_wifi nvs_flash mqtt json app_ota)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    REQUIRES ${pri_req} )

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")