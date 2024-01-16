set(app_src app_ota.c)
set(pri_req driver esp_wifi app_update esp_http_client esp_https_ota app_mqtt )
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    REQUIRES ${pri_req}
                    )

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
