#ifndef APP_MQTT_H
#define APP_MQTT_H


void mqtt_app_start(void);
int mqtt_publish(const char* msg, const char* topic, uint16_t len);
// void create_json_number(char* name_json,int number_args,...);
#endif