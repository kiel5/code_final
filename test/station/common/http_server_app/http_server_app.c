#include "http_server_app.h"

/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_eth.h"
// #include "protocol_examples_common.h"
// #include "esp_tls_crypto.h"
#include <esp_http_server.h>

static const char *TAG = "http_server";        // label
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[]   asm("_binary_index_html_end");


static void app_http_server_post_default_handlder_func(char* buf, int len );       //declaration function
static void app_http_server_get_default_handler_func(char* url_query,char* host);
static http_post_handler_func_t http_post_handler_func = NULL;  // assiger pointer function
static http_get_handler_func_t  http_get_handler_func  = NULL;
static char http_post_buf[APP_HTTP_SERVER_POST_MAX_BUFFER];
static httpd_handle_t http_server;

static httpd_req_t *resp_dht11;
static httpd_req_t *resp;
#if CONFIG_EXAMPLE_BASIC_AUTH

typedef struct {
    char    *username;
    char    *password;
} basic_auth_info_t;

#define HTTPD_401      "401 UNAUTHORIZED"           /*!< HTTP Response 401 */

static char *http_auth_basic(const char *username, const char *password)
{
    int out;
    char *user_info = NULL;
    char *digest = NULL;
    size_t n = 0;
    asprintf(&user_info, "%s:%s", username, password);
    if (!user_info) {
        ESP_LOGE(TAG, "No enough memory for user information");
        return NULL;
    }
    esp_crypto_base64_encode(NULL, 0, &n, (const unsigned char *)user_info, strlen(user_info));

    /* 6: The length of the "Basic " string
     * n: Number of bytes for a base64 encode format
     * 1: Number of bytes for a reserved which be used to fill zero
    */
    digest = calloc(1, 6 + n + 1);
    if (digest) {
        strcpy(digest, "Basic ");
        esp_crypto_base64_encode((unsigned char *)digest + 6, n, (size_t *)&out, (const unsigned char *)user_info, strlen(user_info));
    }
    free(user_info);
    return digest;
}

/* An HTTP GET handler */
static esp_err_t basic_auth_get_handler(httpd_req_t *req)
{
    char *buf = NULL;
    size_t buf_len = 0;
    basic_auth_info_t *basic_auth_info = req->user_ctx;

    buf_len = httpd_req_get_hdr_value_len(req, "Authorization") + 1;
    if (buf_len > 1) {
        buf = calloc(1, buf_len);
        if (!buf) {
            ESP_LOGE(TAG, "No enough memory for basic authorization");
            return ESP_ERR_NO_MEM;
        }

        if (httpd_req_get_hdr_value_str(req, "Authorization", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Authorization: %s", buf);
        } else {
            ESP_LOGE(TAG, "No auth value received");
        }

        char *auth_credentials = http_auth_basic(basic_auth_info->username, basic_auth_info->password);
        if (!auth_credentials) {
            ESP_LOGE(TAG, "No enough memory for basic authorization credentials");
            free(buf);
            return ESP_ERR_NO_MEM;
        }

        if (strncmp(auth_credentials, buf, buf_len)) {
            ESP_LOGE(TAG, "Not authenticated");
            httpd_resp_set_status(req, HTTPD_401);
            httpd_resp_set_type(req, "application/json");
            httpd_resp_set_hdr(req, "Connection", "keep-alive");
            httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Hello\"");
            httpd_resp_send(req, NULL, 0);
        } else {
            ESP_LOGI(TAG, "Authenticated!");
            char *basic_auth_resp = NULL;
            httpd_resp_set_status(req, HTTPD_200);
            httpd_resp_set_type(req, "application/json");
            httpd_resp_set_hdr(req, "Connection", "keep-alive");
            asprintf(&basic_auth_resp, "{\"authenticated\": true,\"user\": \"%s\"}", basic_auth_info->username);
            if (!basic_auth_resp) {
                ESP_LOGE(TAG, "No enough memory for basic authorization response");
                free(auth_credentials);
                free(buf);
                return ESP_ERR_NO_MEM;
            }
            httpd_resp_send(req, basic_auth_resp, strlen(basic_auth_resp));
            free(basic_auth_resp);
        }
        free(auth_credentials);
        free(buf);
    } else {
        ESP_LOGE(TAG, "No auth header received");
        httpd_resp_set_status(req, HTTPD_401);
        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_hdr(req, "Connection", "keep-alive");
        httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Hello\"");
        httpd_resp_send(req, NULL, 0);
    }

    return ESP_OK;
}

static httpd_uri_t basic_auth = {
    .uri       = "/basic_auth",
    .method    = HTTP_GET,
    .handler   = basic_auth_get_handler,
};

static void httpd_register_basic_auth(httpd_handle_t server)
{
    basic_auth_info_t *basic_auth_info = calloc(1, sizeof(basic_auth_info_t));
    if (basic_auth_info) {
        basic_auth_info->username = CONFIG_EXAMPLE_BASIC_AUTH_USERNAME;
        basic_auth_info->password = CONFIG_EXAMPLE_BASIC_AUTH_PASSWORD;

        basic_auth.user_ctx = basic_auth_info;
        httpd_register_uri_handler(server, &basic_auth);
    }
}
#endif

static void app_http_server_post_default_handlder_func(char* buf, int len) // function handler
{
    // log data receive from POST
    ESP_LOGI(TAG, "=========== RECEIVED POST DATA ==========");
    ESP_LOGI(TAG, "%.*s", len, buf);   
    ESP_LOGI(TAG, "====================================");
}

void app_http_server_send_resp(char * buf, int len)  // response for client
{
    /* Send response with custom headers and body set as the
     * string passed in user context*/
    httpd_resp_send(resp, buf, len); // response client

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(resp, "Host") == 0)
    {
        ESP_LOGI(TAG, "Request headers lost");
    }
}
static void app_http_server_get_default_handler_func(char* url_query,char* host)
{
    // log data 
    ESP_LOGI(TAG, "=========== RECEIVED GET PARAM ==========");
    ESP_LOGI(TAG, "url_query: %s, host: %s", url_query, host);  
    app_http_server_send_resp(HTTP_GET_RESPONSE_DEFAULT,sizeof(HTTP_GET_RESPONSE_DEFAULT));
    ESP_LOGI(TAG, "====================================");
}



static esp_err_t http_get_handler(httpd_req_t *req)
{
    // httpd_resp_set_type(req,"text/html");
    // httpd_resp_send(req,(const char*)index_html_start,index_html_end - index_html_start);
    char *host = NULL;
    size_t host_len;
    char *url_query = NULL;
    size_t url_query_len;
    resp = req;
    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    host_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (host_len > 1)
    {
        host = malloc(host_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", host, host_len) == ESP_OK)
        {
            ESP_LOGI(TAG, "Found header => Host: %s", host);
        }
        free(host);
    }

    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    url_query_len = httpd_req_get_url_query_len(req) + 1;
    if (url_query_len > 1)
    {
        url_query = malloc(url_query_len);
        if (httpd_req_get_url_query_str(req, url_query, url_query_len) == ESP_OK)
        {
            free(host);
        }
      //  free(url_query);
    }
    app_http_server_get_default_handler_func(url_query,host);
    if(url_query != NULL) free(url_query);
    if(host != NULL)      free(host);
    return ESP_OK;
}

static const httpd_uri_t http_get = {
    .uri       = APP_HTTP_SERVER_GET_DEFAULT_URI,
    .method    = HTTP_GET,
    .handler   = http_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};

/* An HTTP POST handler */
static esp_err_t http_post_handler(httpd_req_t *req)
{
    //char* buf = http_post_buf;
    char buf[100];
    //int buf_len = sizeof(http_post_buf);
    int ret, remaining = req->content_len;

    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                        MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;
        app_http_server_post_default_handlder_func(buf, req->content_len); // log receive
        http_post_handler_func(buf, req->content_len);

    }
    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t http_post = {
    .uri       = APP_HTTP_SERVER_POST_DEFAULT_URI,
    .method    = HTTP_POST,
    .handler   = http_post_handler,
    .user_ctx  = NULL
}; 

static esp_err_t html_server(httpd_req_t *req)
{
    httpd_resp_set_type(req,"text/html");
    httpd_resp_send(req,(const char*)index_html_start,index_html_end - index_html_start);
    return ESP_OK;
}
static const httpd_uri_t get_html = {
    .uri       = "/server",
    .method    = HTTP_GET,
    .handler   = html_server,
    .user_ctx  = NULL
};
void app_http_start_webserver(void)
{
    // httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&http_server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(http_server, &http_post);
        httpd_register_uri_handler(http_server, &http_get);
        httpd_register_uri_handler(http_server, &get_html);
        #if CONFIG_EXAMPLE_BASIC_AUTH
        httpd_register_basic_auth(server);
        #endif
        return ;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return ;
}

void app_http_stop_webserver(void)
{
    if(http_server)
    {
        httpd_stop(http_server);
        http_server = NULL;
    }
}

void app_http_server_set_post_callback(void* post_handler_callback)
{
    if(post_handler_callback != NULL)
    {
        http_post_handler_func = post_handler_callback;
    }
}
void app_http_server_set_get_callback(void* get_handler_callback)
{
    if(get_handler_callback != NULL)
    {
        http_get_handler_func = get_handler_callback;
    }
}












