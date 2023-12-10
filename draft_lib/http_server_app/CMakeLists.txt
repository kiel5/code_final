set(app_src http_server_app.c)
set(pri_req)
set(IDF_PATH C:/Users/kien/Downloads/esp32/esp-idf-v5.0)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    REQUIRES ${pri_req} nvs_flash esp_wifi esp_eth esp_http_server)

