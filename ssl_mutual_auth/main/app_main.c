
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

#include "app_config.h"
#include "http_server_app.h"
#include "app_mqtt.h"
#include "sensor.h"
#include "i2c_oled.h"




static const char *TAG_mqtt = "sonoff";


void app_main(void)
{
    ESP_LOGI(TAG_mqtt, "[APP] Startup..");
    ESP_LOGI(TAG_mqtt, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG_mqtt, "[APP] IDF version: %s", esp_get_idf_version());

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_LOGI(TAG_mqtt, "done");
//    app_config();  // hoàn thành việc connect wifi
//    mqtt_app_start();

//    pcnt_init();

	char lineChar[20] = "kienkm59";
    char lineChar1[20] = "hello world!";
    oled_init();
    oled_display_text(lineChar, 0, strlen(lineChar));
    oled_display_text(lineChar1,1, strlen(lineChar1));
    oled_display_text(lineChar1, 0, strlen(lineChar1));
    oled_display_horizental(lineChar1, 3,strlen(lineChar1));

}
