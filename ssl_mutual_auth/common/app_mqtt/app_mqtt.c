
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

#include "esp_log.h"
#include "mqtt_client.h"
#include "cJSON.h"
#include "app_ota.h"

#define ACCESS_TOKEN_NODE1 "B7XQ2etAsRrFpRuNHWf6"
#define ACCESS_TOKEN_NODE2 "1aBMYP6XqgXqNhIqxKcd"
const char *ACCESS_TOKEN_DEVICE = ACCESS_TOKEN_NODE1;

#define BROKER_URI "mqtt://demo.thingsboard.io"
#define BROKER_PORT 1883

static const char *TAG = "MQTTS_APP";
static const char *tag_json = "json";

esp_mqtt_client_handle_t client;

extern otaConfig otaC;

extern const uint8_t client_cert_pem_start[] asm("_binary_client_crt_start");
extern const uint8_t client_cert_pem_end[] asm("_binary_client_crt_end");
extern const uint8_t client_key_pem_start[] asm("_binary_client_key_start");
extern const uint8_t client_key_pem_end[] asm("_binary_client_key_end");
extern const uint8_t server_cert_pe_mstart[] asm("_binary_mosquitto_org_crt_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_mosquitto_org_crt_end");

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}
static bool check_version_equal(const char *fw_current, const char *fw_update)
{
    if (fw_current == NULL || fw_update == NULL)
    {
        // Xử lý trường hợp đầu vào bị NULL
        ESP_LOGE(TAG, "Version strings cannot be NULL");
        return false;
    }

    int cmp_result = strcmp(fw_current, fw_update);

    if (cmp_result == 0)
    {
        // Phiên bản bằng nhau
        return true;
    }
    else
    {
        // Phiên bản khác nhau
        return false;
    }
}
static bool check_url(otaConfig otaC)
{
    if (strlen(otaC.url) == 0)
    {
        ESP_LOGW(TAG, "Firmware URL is not specified");
        return false;
    }

    if (strlen(otaC.url) == 0)
    {
        ESP_LOGW(TAG, "Target firmware version is not specified");
        return false;
    }
    if (strlen(otaC.cert) == 0)
    {
        ESP_LOGW(TAG, "Target firmware version is not specified");
        return false;
    }
    return true;
}
int mqtt_publish(const char *msg, uint16_t len)
{
    int msgid;
    msgid = esp_mqtt_client_publish(client, "v1/devices/me/telemetry", msg, 0, 1, 0);
    return msgid;
}
void receivemsg_task(esp_mqtt_event_handle_t event)
{
    printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
    printf("DATA =%.*s\r\n", event->data_len, event->data);

    if (strncmp(event->topic, "v1/devices/me/attributes", event->topic_len) == 0)
    {
        ESP_LOGI(TAG, "ota_event_receive_url");
        // kick off OTA update
        const cJSON *msg = cJSON_Parse(event->data);
        if (msg != NULL)
        {
            const cJSON *data = cJSON_GetObjectItemCaseSensitive(msg, "update_ota");
            cJSON *url = cJSON_GetObjectItemCaseSensitive(data, "URL");
            cJSON *cert = cJSON_GetObjectItemCaseSensitive(data, "cert");
            cJSON *version = cJSON_GetObjectItemCaseSensitive(data, "version");
            if (url != NULL && cert != NULL && version != NULL)
            {
                strcpy(otaC.url, url->valuestring);
                strcpy(otaC.cert, cert->valuestring);
                strcpy(otaC.version, version->valuestring);
                cJSON_Delete((cJSON *)msg);
            }
            else
            {
                cJSON *root = cJSON_CreateObject();
                cJSON_AddStringToObject(root, "OTA_Err", "invalid OTA parameter URL or cert or version");
                char *buffer = cJSON_Print(root);
                cJSON_Delete(root);
                int msg_id = mqtt_publish(buffer, strlen(buffer));
                free(buffer);
            }
        }
        else if (msg == NULL)
        {
            ESP_LOGE(TAG, "Invalid JSON format null");
            cJSON *root = cJSON_CreateObject();
            cJSON_AddStringToObject(root, "OTA_Err", "error parse json");
            char *buffer = cJSON_Print(root);
            cJSON_Delete(root);
            int msg_id = mqtt_publish(buffer, strlen(buffer));
            free(buffer);
        }
        if (!check_version_equal(PROJECT_VERSION, otaC.version) && check_url(otaC))
        {

            printf("OTA from %s, cert=%s\r\n", otaC.url, otaC.cert);
            xTaskCreate(&app_ota, "app_ota_task", 8192, NULL, 5, NULL);
        }
        else
        {
            ESP_LOGI(TAG, "err update ota");
            // publish version firmware
            cJSON *root = cJSON_CreateObject();
            cJSON_AddStringToObject(root, "OTA_Err", "error version OTA match or url is incorrect");
            char *buffer = cJSON_Print(root);
            cJSON_Delete(root);
            int msg_id = mqtt_publish(buffer, strlen(buffer));
            free(buffer);
        }
    }

    else /* default: */
    {
        ;
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_subscribe(client, "v1/devices/me/attributes", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        // publish version firmware
        cJSON *root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "fw_version", PROJECT_VERSION);
        char *buffer = cJSON_Print(root);
        cJSON_Delete(root);
        msg_id = mqtt_publish(buffer, strlen(buffer));
        free(buffer);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        receivemsg_task(event);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}
void mqtt_app_start(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = BROKER_URI,
        .broker.address.port = BROKER_PORT,
        //.credentials.client_id = "e7a01ae0-a235-11ee-a189-a96dbb3173db",
        .credentials.username = ACCESS_TOKEN_DEVICE,
        // .broker.verification.certificate = (const char *)client_cert_pem_start,
        // .credentials = {
        //   .authentication = {
        //     .certificate = (const char *)client_cert_pem_start,
        //     .key = (const char *)client_key_pem_start,
        //   },
        // }
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}
