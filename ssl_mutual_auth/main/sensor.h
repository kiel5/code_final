#ifndef SENSOR_H
#define SENSOR_H
#include "stdio.h"
#include "esp_log.h"


#define EXAMPLE_PCNT_HIGH_LIMIT 100    // tự động reset về 0 khi vượt mức 
#define EXAMPLE_PCNT_LOW_LIMIT  -100
#define EXAMPLE_EC11_GPIO_A 23

void pcnt_init(void);



#endif