#ifndef I2C_OLED_H
#define I2C_OLED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"


void oled_init(void);
void oled_display_text(char *data,int row,int length);
void oled_display_horizental(char *data, int row, int length);
 
#endif