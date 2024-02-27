/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "cJSON.h"
#include "string.h"


const char* TAG = "dmm";

int app_main(void)
{
ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());
// cJSON *root = cJSON_CreateObject();

// // Thêm đối tượng con 'user' vào root
// cJSON *user = cJSON_AddObjectToObject(root, "user"); 

// // Thiết lập giá trị cho đối tượng con
// cJSON_AddStringToObject(user, "name", "https://raw.githubusercontent.com/kiel5/ota_fw/master/blink.bin");
// cJSON_AddNumberToObject(user, "age", 30);
// cJSON_AddStringToObject(user, "sex", "male");

//     char* buff1 = cJSON_Print(root);
//     char*buff2  = cJSON_Print(user);

//     cJSON_Delete(root);
//     printf("%s\n _ %s",buff1, buff2);

// cJSON* msg = cJSON_Parse(buff1);
// cJSON* key_all = cJSON_GetObjectItem(msg,"user");
// cJSON* key1   = cJSON_GetObjectItem(key_all,"name");
// cJSON* key2    = cJSON_GetObjectItem(key_all,"age");
// cJSON* key3    = cJSON_GetObjectItem(key_all,"sex");
// char buff3[50];
// char buff5[100];
// int buff4;
// strcpy(buff3,key1->valuestring);
// strcpy(buff5,key3->valuestring);
// buff4 = key2->valueint;
// printf("dcmm:%s\r\n",buff3);
// printf("dmm:%d\r\n",buff4);
// printf("dmm1:%s\r\n",buff5);
// cJSON_Delete(msg);
return 0;
}
