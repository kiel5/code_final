
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "string.h"

#include "app_ota.h"
#include "app_mqtt.h"
#include "cJSON.h"
#include <sys/socket.h>

#define HASH_LEN 32
static const char *TAG = "OTA_APP";

otaConfig otaC;
#define OTA_URL_SIZE 256
extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
        break;
    }
    return ESP_OK;
}

esp_http_client_config_t my_config = {
    // .url = "https://raw.githubusercontent.com/kiel5/ota_fw/master/blink.bin",  // đường dẫn chứa file
    // .url ="https://raw.githubusercontent.com/thingsboard/esp32-ota/master/firmware/example-v1.1.bin",
    .url = otaC.url,
    .cert_pem = (char *)server_cert_pem_start,
    .event_handler = _http_event_handler,
    .keep_alive_enable = true,
    .skip_cert_common_name_check = false,
};
static void print_sha256(const uint8_t *image_hash, const char *label)
{
    char hash_print[HASH_LEN * 2 + 1];
    hash_print[HASH_LEN * 2] = 0;
    for (int i = 0; i < HASH_LEN; ++i)
    {
        sprintf(&hash_print[i * 2], "%02x", image_hash[i]);
    }
    ESP_LOGI(TAG, "%s %s", label, hash_print);
}

static void get_sha256_of_partitions(void)
{
    uint8_t sha_256[HASH_LEN] = {0};
    esp_partition_t partition;

    // get sha256 digest for bootloader
    partition.address = ESP_BOOTLOADER_OFFSET;
    partition.size = ESP_PARTITION_TABLE_OFFSET;
    partition.type = ESP_PARTITION_TYPE_APP;
    esp_partition_get_sha256(&partition, sha_256);
    print_sha256(sha_256, "SHA-256 for bootloader: ");

    // get sha256 digest for running partition
    esp_partition_get_sha256(esp_ota_get_running_partition(), sha_256);
    print_sha256(sha_256, "SHA-256 for current firmware: ");
}

void app_ota(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting OTA example task");
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "OTA_Status", "start update ota");
    cJSON_AddStringToObject(root, "OTA_URL", otaC.url);
    char *buffer = cJSON_Print(root);
    cJSON_Delete(root);
    int msg_id = mqtt_publish(buffer, strlen(buffer));
    free(buffer);
    get_sha256_of_partitions();
    esp_http_client_config_t *config = &my_config;
    // if (!config)
    // {
    //     ESP_LOGI(TAG, "esp_http_client config not found ");
    //     return ESP_ERR_INVALID_ARG;
    // }

    esp_https_ota_config_t ota_config = {
        .http_config = config,
    };
    ESP_LOGI(TAG, "Attempting to download update from %s", config->url);
    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "OTA Succeed, Rebooting...");
        cJSON *root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "OTA_status", "done update ota");
        cJSON_AddStringToObject(root, "OTA_version_lastupdate", otaC.version);
        char *buffer = cJSON_Print(root);
        cJSON_Delete(root);
        int msg_id = mqtt_publish(buffer, strlen(buffer));
        if (msg_id >= 0)
        {
            free(buffer);
            printf("prepare restart");
            esp_restart();
        }
        else
        {
            free(buffer);
            printf("Publish failed, restarting in 2s");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            esp_restart();
        }
    }
    else
    {
        ESP_LOGE(TAG, "Firmware upgrade failed");
        cJSON *root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "OTA_Status", "fail");
        char *buffer = cJSON_Print(root);
        cJSON_Delete(root);
        int msg_id = mqtt_publish(buffer, strlen(buffer));
        free(buffer);
    }
}
