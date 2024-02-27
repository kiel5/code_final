set(app_src sensor_flow.c)
# set(pri_req driver esp_timer)
set(pri_req)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    REQUIRES ${pri_req})

