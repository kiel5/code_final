#ifndef INPUT_IO_H
#define INPUT_IO_H
#include "stdio.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "driver/gpio.h"

typedef enum{
    LO_TO_HI = 1, // rising 
    HI_TO_LO = 2, // falling
    ANY_EDLE = 3  // both
} interrupt_type_edle_t;

typedef void (*input_callback_t)(int);

void input_io_create(gpio_num_t gpio_num, interrupt_type_edle_t type);
int input_io_get_level(gpio_num_t gpio_num);
void input_set_callback(void *cb);

#endif