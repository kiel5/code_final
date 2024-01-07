set(app_src i2c_oled.c ssd1306_i2c.c ssd1306_spi.c ssd1306.c)
set(pri_req driver)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    REQUIRES ${pri_req})

