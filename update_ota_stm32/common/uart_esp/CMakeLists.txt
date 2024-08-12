set(app_src uart_esp.c)
set(pri_req driver ota_command min_protocol)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    REQUIRES ${pri_req}
                    )

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
