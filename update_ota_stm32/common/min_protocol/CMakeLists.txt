set(app_src min_port.c min.c)
set(pri_req driver uart_esp)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    REQUIRES ${pri_req}
                    )

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
