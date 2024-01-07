
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


#define ACCESS_TOKEN_NODE1 "B7XQ2etAsRrFpRuNHWf6"
#define ACCESS_TOKEN_NODE2 "1aBMYP6XqgXqNhIqxKcd"
const char*    ACCESS_TOKEN_DEVICE    = ACCESS_TOKEN_NODE1;

#define BROKER_URI         "mqtt://demo.thingsboard.io"
#define BROKER_PORT        1883

static const char *TAG      = "MQTTS_APP";
static const char *tag_json = "json";

esp_mqtt_client_handle_t client;


// const uint16_t Thingsboard_port = 1883;

extern const uint8_t client_cert_pem_start[] asm("_binary_client_crt_start");
extern const uint8_t client_cert_pem_end  [] asm("_binary_client_crt_end");
extern const uint8_t client_key_pem_start [] asm("_binary_client_key_start");
extern const uint8_t client_key_pem_end   [] asm("_binary_client_key_end");
extern const uint8_t server_cert_pe_mstart[] asm("_binary_mosquitto_org_crt_start");
extern const uint8_t server_cert_pem_end  [] asm("_binary_mosquitto_org_crt_end");

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}
int mqtt_publish(const char* msg, const char* topic, uint16_t len)
{
  int msgid;
  msgid = esp_mqtt_client_publish(client, topic , msg, 0, 1, 0);
  return msgid;
}
void receivemsg_task(int len, char *msg)
{
    /*{"method": "setValue", 
       "params": {"x": 100, "y":200}}
    */
  printf("%s", msg);

  cJSON *method;
  cJSON *params;
  cJSON *root = cJSON_Parse(msg);
  method = cJSON_GetObjectItem(root, "method");
  params = cJSON_GetObjectItem(root, "params");
  
  printf("%s\n", method->valuestring);
  printf("%s\n", params->valuestring);

    // if(strcmp(method->valuestring,"setValue")==0)
    // {
        
    //     limiar_accel_x=atoi(params->valuestring);
    // }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_subscribe(client, "v1/devices/me/telemetry", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
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
        // printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        // printf("DATA =%.*s\r\n", event->data_len, event->data);
        receivemsg_task(event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
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
    .broker.address.uri   = BROKER_URI,
    .broker.address.port  = BROKER_PORT,
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


