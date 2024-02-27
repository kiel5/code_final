#ifndef APP_OTA_H
#define APP_OTA_H

#define PROJECT_VERSION "1.0.0"
void app_ota(void *pvParameter);

typedef struct {
    char url[100];
    char cert[1500];
    char version[15];
}otaConfig;

#endif