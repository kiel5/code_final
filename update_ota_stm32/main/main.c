
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
#include "app_config.h"
#include "app_ota.h"
#include "bootloader_command.h"
#include "uart_esp.h"

#include "esp_idf_version.h"
static const char *TAG = "final";

#include "min.h"

void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());

    //app_config();
    ESP_LOGI(TAG,"config_uart");
    app_uart_init();
    ESP_LOGI(TAG,"config_uart_done");
    bootloader_init();

    //int kien = uart_write_bytes(0,"d",4);
    //xTaskCreate(&app_ota, "app_ota_task", 8192, NULL, 5, NULL);
    // stm32 send to esp32 by uart0, esp32 send uart0 to print terminal
    //chua goi min_poll
    // xảy ra ngắt uart thì gọi min_poll, truyền vào data luôn, đỡ tốn công

    // lỗi kí tự là hex
    //xử lý gọi minpoll mà không vào handler
}
