set(app_src app_nvs.c)
set(pri_req driver nvs_flash)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    REQUIRES ${pri_req})

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")