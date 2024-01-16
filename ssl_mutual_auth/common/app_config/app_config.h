#ifndef APP_CONFIG_H
#define APP_CONFIG_H
#include <stdbool.h>
#include <stdint.h>

typedef enum{
    PROVISION_ACCESSPOINT = 0,
    PROVISION_SMARTCONFIG = 1
}provision_type_t;

bool is_provisioned(void);   // check xem được cấu hình wifi chưa
void app_config(void);
void ap_start();
void http_post_data_callback(char* buf, uint16_t len);
#endif