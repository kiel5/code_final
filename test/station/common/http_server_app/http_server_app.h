#ifndef __HTTP_SERVER_APP_H
#define __HTTP_SERVER_APP_H
#include "stdint.h"
#include "esp_http_server.h"
#include "string.h"
// typedef void(*http_post_callback_t)(char* dta,uint16_t len);
// typedef void(*http_get_callback_t)(void);
// void http_set_callback_dht11(void* cb);
// void http_set_callback_switch(void *cb);
// void http_set_wifi_info_callback(void* cb);
// void start_webserver(void);
// void stop_webserver(void );
// void dht11_response(char * data, int len );

enum 
{
    HTTP_GET_RECEIVE_HEADER = 1,
    HTTP_GET_RECEIVE_QUERY,
    HTTP_GET_RECEIVE_LOST
};

#define APP_HTTP_SERVER_GET_DEFAULT_URI   "/get"
#define APP_HTTP_SERVER_POST_DEFAULT_URI  "/post"
#define APP_HTTP_SERVER_PUT_DEFAULT_URI   "/put"
#define HTTP_GET_RESPONSE_DEFAULT         "ABCD"
#define APP_HTTP_SERVER_POST_MAX_BUFFER    100

typedef void(*http_post_handler_func_t)(char*, int);
typedef void(*http_get_handler_func_t)(char* url_query,char* host);


void app_http_server_set_get_callback(void* get_handler_callback);
void app_http_server_set_post_callback(void* post_handler_callback);
void app_http_stop_webserver(void);
void app_http_start_webserver(void);

#endif
