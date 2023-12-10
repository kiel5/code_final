#ifndef OUTPUT_IO_H
#define OUTPUT_IO_H
#include "stdio.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "driver/gpio.h"

void output_io_toggle(gpio_num_t gpio_num);
void output_io_set_level(gpio_num_t gpio_num, int level);
void output_io_create(gpio_num_t gpio_num);
#endif