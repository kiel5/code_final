
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "stdlib.h" 
#include "esp_log.h"

#include "http_server_app.h"
#include "app_mqtt.h"
#include "sensor.h"
#include "i2c_oled.h"
#include "app_config.h"
#include "app_ota.h"
#include "app_nvs.h"
#include "cJSON.h"
#include "esp_idf_version.h"
static const char *TAG = "final";

#define KEY1 "restart_str"


void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    //ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_LOGI(TAG, "done");

    oled_init();
    app_config(); 
    mqtt_app_start();   
    sensor_init();

}
