#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include "esp_system.h"
#include "driver/gpio.h"
#include <sys/time.h>
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_spiffs.h"
#include <sys/stat.h>

#include "sensor.h"
#include "input_iot.h"
#include "app_mqtt.h"
#include "i2c_oled.h"
#include "cJSON.h"
#include "app_nvs.h"

// #include "esp_event_loop.h"
#include "esp_log.h"

#define PULSEPERLITRE 450
#define ADC_READ_PIN ADC_CHANNEL_7
#define PERSIST_DIFFERENCE 500
#define PERSIST_PERIOD 600

TickType_t delay_1Sec = 1000 / portTICK_PERIOD_MS;
TickType_t delay_5Sec = 5000 / portTICK_PERIOD_MS;
TickType_t delay_100ms = 100 / portTICK_PERIOD_MS;

long protectedCurrPulses = 0L;
int pulse_count = 0;
bool flowing = false;

static const char *TAG = "sensor";
static const char *TAG_MQTT = "MQTT";

static QueueHandle_t qPulse, qReport_flow, qReport_pin, qPersistPulses;
SemaphoreHandle_t sCountSem;

typedef struct
{
    struct timeval tme;
    float lits;
    float rate;
} report_sensor;
long getCurrentPulses()
{
    long ret = 0;
    xSemaphoreTake(sCountSem, portMAX_DELAY);
    ret = protectedCurrPulses;
    xSemaphoreGive(sCountSem);
    return ret;
}
void setCurrentPulses(long p)
{
    xSemaphoreTake(sCountSem, portMAX_DELAY);
    protectedCurrPulses = p;
    xSemaphoreGive(sCountSem);
}

void IRAM_ATTR sensor_callback()
{
    pulse_count++;
    xQueueSendToBackFromISR(qPulse, &pulse_count, (TickType_t)0);
}
static bool adc_calibration_init(adc_unit_t unit, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;
    if (!calibrated)
    {
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK)
        {
            calibrated = true;
        }
    }
    *out_handle = handle;
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "ADC: Calibration Success with Line Fitting");
    }
    else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated)
    {
        ESP_LOGW(TAG, "ADC: eFuse not burnt, skip software calibration");
    }
    else
    {
        ESP_LOGE(TAG, "ADC: Invalid arg or no memory");
    }
    return calibrated;
}

static void adc_calibration_deinit(adc_cali_handle_t handle)
{
    ESP_LOGI(TAG, "ADC: deregister %s calibration scheme", "Line Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
}

void task_read_pin(void *pvParameter)
{
    //-------------ADC1 Init---------------//
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_2_5,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_READ_PIN, &config));

    //-------------ADC1 Calibration Init---------------//
    adc_cali_handle_t adc1_cali_handle = NULL;
    adc_calibration_init(ADC_UNIT_1, ADC_ATTEN_DB_2_5, &adc1_cali_handle);
    int adc_raw;
    int voltage;
    ESP_LOGI(TAG, "Starting read_pin_task");
    while (1)
    {
        int counter = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_READ_PIN, &adc_raw));
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_handle, adc_raw, &voltage));
        ESP_LOGI(TAG, "ADC: ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, ADC_READ_PIN, voltage);
        float battery = voltage * 11 * 0.001;
        int Capacity = (int)(-96.63 * battery * battery * battery + 999.34 * battery * battery - 3278.71 * battery + 3400.59);
        if (Capacity <= 100 && Capacity >= 0)
        {
            xQueueSendToBack(qReport_pin, &Capacity, (TickType_t)0);
        }
        else
        {
            counter++;
            if (counter == 5)
            {
                int var_fail = -5;
                xQueueSendToBack(qReport_pin, &var_fail, (TickType_t)0);
                counter = 0;
            }
        }
        vTaskDelay(delay_5Sec);
    }
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));
    adc_calibration_deinit(adc1_cali_handle);
    vTaskDelete(NULL);
}
void task_report_pin(void *pvParameter)
{
    int capacity = 0;
    ESP_LOGI(TAG, "Starting report_pin_task");
    while (1)
    {
        //-------------wait data from task read_pin---------------//
        ESP_LOGI(TAG, "Waiting on reporting capacity battery");
        xQueueReceive(qReport_pin, &capacity, portMAX_DELAY);

        if (capacity == -5)
        {
            ESP_LOGI(TAG, "read_battery fail");
            oled_display_text("read_pin fail", 3, strlen("read_pin fail"));

            //-------------publish mqtt---------------//
            cJSON *data = cJSON_CreateObject();
            cJSON_AddStringToObject(data, "Cacacity_battery", "fail");
            char *buffer = cJSON_Print(data);
            int msg_id = mqtt_publish(buffer, strlen(buffer));
            ESP_LOGI(TAG_MQTT, "sent publish capacity fail, msg_id=%d", msg_id);
            cJSON_Delete(data);
            free(buffer);
        }
        else
        {
            //-------------display capacity battery in oled---------------//
            ESP_LOGI(TAG, "capacity battery: %d", capacity);
            char str_capacity[14] = "";
            sprintf(str_capacity, "node1_pin  %d%%", capacity);
            oled_display_text(str_capacity, 3, strlen(str_capacity));

            //-------------publish mqtt---------------//
            cJSON *data = cJSON_CreateObject();
            cJSON_AddNumberToObject(data, "Cacacity_battery", capacity);
            char *buffer = cJSON_Print(data);
            cJSON_Delete(data);
            int msg_id = mqtt_publish(buffer, strlen(buffer));
            ESP_LOGI(TAG_MQTT, "sent publish capacity battery successful, msg_id=%d", msg_id);
            free(buffer);
        }
    }
    vTaskDelete(NULL);
}
void task_reportFlow(void *pvParameter)
{

    report_sensor report;
    int msg_id;
    struct timeval now;
    long previous_timepub = 0;
    long tme_gap = 0;
    ESP_LOGI(TAG, "Starting report_flow_task");
    while (1)
    {
        //-------------wait data from task read_sensor---------------//
        ESP_LOGI(TAG, "Waiting on reporting Flow queue");
        xQueueReceive(qReport_flow, &report, portMAX_DELAY);
        gettimeofday(&now, 0);
        tme_gap = now.tv_sec - previous_timepub;
        //-------------add data to json---------------//
        // cJSON *json = cJSON_CreateObject();
        cJSON *data = cJSON_CreateObject();
        cJSON_AddStringToObject(data, "device", "node1");
        cJSON_AddNumberToObject(data, "Litres", report.lits);
        cJSON_AddNumberToObject(data, "Rate", report.rate);
        // cJSON_AddItemToObject(json, "d", data);
        char *buffer = cJSON_Print(data);
        cJSON_Delete(data);
        ESP_LOGI(TAG, "Reporting json : %s", buffer);
        //-------------display data sensor in oled---------------//
        char str_value_rate[14] = "";
        char str_value_lits[14] = "";
        sprintf(str_value_lits, "node1_lits: %0.3f", report.lits);
        sprintf(str_value_rate, "node1_rate: %0.3f", report.rate);
        // oled_display_text("devide: node1", 0, strlen("devide: node1"));
        oled_display_text(str_value_rate, 1, strlen(str_value_rate));
        oled_display_text(str_value_lits, 2, strlen(str_value_lits));
        //--------------publish data sensor with cycle 15s--------------//
        if (tme_gap > 15)
        {
            previous_timepub = now.tv_sec;
            msg_id = mqtt_publish(buffer, strlen(buffer));
            ESP_LOGI(TAG_MQTT, "sent publish volume and rate successful, msg_id=%d", msg_id);
        }
        free(buffer);
    }
    vTaskDelete(NULL);
}
void task_Flow(void *pvParameter)
{
    //--------------set gpio input pin, set callback interrupt--------------//
    input_io_create(23, HI_TO_LO);
    input_set_callback(sensor_callback);

    long pulses, previousReportingPulses, previousReportingTime;
    float previousReportingRate; // lưu lượng trước đó
    struct timeval now;
    report_sensor report;
    long interval = 0;
    pulses = getCurrentPulses();
    previousReportingRate = 0.0;
    previousReportingPulses = pulses; // số xung trước đó
    previousReportingTime = 0;
    ESP_LOGI(TAG, "Starting flow_task");
    while (1)
    {
        //--------------wait pulse from interrupt callback--------------//
        ESP_LOGV(TAG, "Waiting on interrupt queue");
        BaseType_t rc = xQueueReceive(qPulse, &pulses, delay_1Sec);
        if (errQUEUE_EMPTY == rc) // block 1s for wait pulse
        {
            ESP_LOGD(TAG, "Timeout waiting for pulses, pulses count = %d", pulses);
        }
        //-------------write value pulses to file or nvs---------------//
        else
        {
            setCurrentPulses(pulses);
            ESP_LOGI(TAG, "pulses:%d", pulses);
            xQueueSendToBack(qPersistPulses, &pulses, (TickType_t)0);
        }
        gettimeofday(&now, 0);
        interval = now.tv_sec - previousReportingTime;
        if ((0 > interval) || (32 < interval))
        {
            ESP_LOGW(TAG, "Interval out of bounds = %li", interval);
            interval = 0;
            previousReportingTime = now.tv_sec;
        }
        if (pulses == previousReportingPulses)
        {
            if (true == flowing)
            { // nước bắt đầu dừng
                ESP_LOGI(TAG, "Flow stopped");
                // xQueueSendToBack(qStoppedFlow, &pulses, (TickType_t)0);
                flowing = false;
            }
            // no water has been used
            if ((interval > 30L) || (previousReportingRate > 0.01))
            {
                // 30 second reporting when idle
                report.tme.tv_sec = now.tv_sec;
                report.tme.tv_usec = now.tv_usec;
                report.lits = (float)pulses / PULSEPERLITRE;
                report.rate = 0.0;
                ESP_LOGD(TAG, "Sending reporting data");
                xQueueSendToBack(qReport_flow, &report, (TickType_t)0);
                previousReportingTime = now.tv_sec;
                previousReportingPulses = pulses;
                previousReportingRate = 0.0;
                // give persist task chance to persist latest value
                xQueueSendToBack(qPersistPulses, &pulses, (TickType_t)0);
            }
        }
        else
        {
            flowing = true;
            if (interval > 1L)
            {
                // water being used - 1 second reporting
                report.tme.tv_sec = now.tv_sec;
                report.tme.tv_usec = now.tv_usec;
                report.lits = (float)pulses / PULSEPERLITRE;
                report.rate = ((float)(pulses - previousReportingPulses) / PULSEPERLITRE) * 60 / interval;
                xQueueSendToBack(qReport_flow, &report, (TickType_t)0);
                previousReportingTime = now.tv_sec;
                previousReportingPulses = pulses;
                previousReportingRate = report.rate;
            }
        }
    }
    vTaskDelete(NULL);
}

void persist_pulse(long pulses)
{
    if (!esp_spiffs_mounted("config"))
    {
        ESP_LOGE(TAG, "persist_pulse: Config partition not mounted");
    }
    if (rename("/fs/pulses.txt", "/fs/pulses_old.txt") != 0)
    {
        ESP_LOGE(TAG, "Rename failed");
    }
    FILE *f = fopen("/fs/pulses.txt", "wb");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    fwrite(&pulses, sizeof pulses, 1, f);
    fclose(f);
    remove("/fs/pulses_old.txt");
    ESP_LOGI(TAG, "Pulse file written");
}
long read_pulse()
{
    long ret = 0L;
    if (!esp_spiffs_mounted("config"))
    {
        ESP_LOGE(TAG, "persist_pulse: Config partition not mounted");
    }
    // Check if pulses file exists
    struct stat st;
    if (stat("/fs/pulses.txt", &st) == 0)
    {
        FILE *f = fopen("/fs/pulses.txt", "rb");
        if (f == NULL)
        {
            ESP_LOGE(TAG, "Failed to open pulses file for reading");
            f = fopen("/fs/pulses_old.txt", "rb");
            if (f == NULL)
            {
                return 0L;
            }
        }
        size_t s = fread(&ret, sizeof ret, 1, f);
        fclose(f);
        if (s == 0)
        {
            f = fopen("/fs/pulses_old.txt", "rb");
            if (f == NULL)
            {
                return 0L;
            }
            size_t s = fread(&ret, sizeof ret, 1, f);
            fclose(f);
            if (s == 0)
            {
                return 0L;
            }
        }
    }
    else
    {
        // no pulses file exists, create one
        ESP_LOGW(TAG, "No pulses file available");
        FILE *f = fopen("/fs/pulses.txt", "wb");
        if (f == NULL)
        {
            ESP_LOGE(TAG, "Failed to open file for writing");
            return 0L;
        }
        fwrite(&ret, sizeof ret, 1, f);
        fclose(f);
    }
    ESP_LOGI(TAG, "Pulse file read = %li", ret);
    return ret;
}

void task_persist(void *pvParameters)
{
    long pulses, nextPersisPulses, nextPersistTime, lastPersistPulses;
    struct timeval now;
    ESP_LOGI(TAG, "Starting persist task");

    gettimeofday(&now, 0);
    nextPersistTime = now.tv_sec + PERSIST_PERIOD;
    lastPersistPulses = getCurrentPulses();
    nextPersisPulses = lastPersistPulses + PERSIST_DIFFERENCE;

    while (1)
    {
        ESP_LOGD(TAG, "Waiting on persit Pulses queue");
        xQueueReceive(qPersistPulses, &pulses, portMAX_DELAY);
        gettimeofday(&now, 0);
        // don't persist too often
        // (big change in pulse count or a period of time since last persist)
        if ((now.tv_sec > nextPersistTime) || (pulses > nextPersisPulses))
        {
            // don't persist if value hasn't changed
            if (lastPersistPulses != pulses)
            {
                persist_pulse(pulses);
                lastPersistPulses = pulses;
                nextPersisPulses = lastPersistPulses + PERSIST_DIFFERENCE;
                nextPersistTime = now.tv_sec + PERSIST_PERIOD;
            }
        }
    }
    esp_vfs_spiffs_unregister(NULL);
    ESP_LOGI(TAG, "SPIFFS unmounted");
    vTaskDelete(NULL);
}
void sensor_init(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    //*********************spiffs init********************************//
    esp_vfs_spiffs_conf_t fs_conf = {
        .base_path = "/fs",
        .partition_label = "config",
        .max_files = 5,
        .format_if_mount_failed = true};
    esp_err_t ret = esp_vfs_spiffs_register(&fs_conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
    }
    size_t total = 0, used = 0;
    ret = esp_spiffs_info("config", &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    BaseType_t rc;

    sCountSem = xSemaphoreCreateBinary();
    if (NULL != sCountSem)
    {
        //**********************read old pulse from flash****************************//
        protectedCurrPulses = read_pulse(); // đọc giá trị pulse cũ từ flash
        pulse_count = protectedCurrPulses;
        xSemaphoreGive(sCountSem);

        //*********************queue init********************************//
        qPulse = xQueueCreate(10, sizeof(long));
        if (NULL == qPulse)
        {
            ESP_LOGE(TAG, "Failed to create qPulse queue");
            esp_restart();
        }
        qReport_flow = xQueueCreate(10, sizeof(report_sensor));
        if (NULL == qReport_flow)
        {
            ESP_LOGE(TAG, "Failed to create qReport_flow queue");
            esp_restart();
        }
        qReport_pin = xQueueCreate(10, sizeof(long));
        if (NULL == qReport_pin)
        {
            ESP_LOGE(TAG, "Failed to create qReport_pin queue");
            esp_restart();
        }
        qPersistPulses = xQueueCreate(5, sizeof(long));
        if (NULL == qPersistPulses)
        {
            ESP_LOGE(TAG, "Failed to create qPersistPulses queue");
            esp_restart();
        }
        //*********************create task********************************//
        rc = xTaskCreate(&task_Flow, "flow", 4096, NULL, 7, NULL);
        if (pdPASS != rc)
        {
            ESP_LOGE(TAG, "Failed to create Flow task");
            esp_restart();
        }
        rc = xTaskCreate(&task_read_pin, "read_pin", 4096, NULL, 6, NULL);
        if (pdPASS != rc)
        {
            ESP_LOGE(TAG, "Failed to create Read_pin task");
            esp_restart();
        }
        rc = xTaskCreate(&task_reportFlow, "report_flow", 4096, NULL, 5, NULL);
        if (pdPASS != rc)
        {
            ESP_LOGE(TAG, "Failed to create report sensor task");
            esp_restart();
        }
        rc = xTaskCreate(&task_report_pin, "report_pin", 2048, NULL, 5, NULL);
        if (pdPASS != rc)
        {
            ESP_LOGE(TAG, "Failed to create report_pin task");
            esp_restart();
        }
        rc = xTaskCreate(&task_persist, "export_txt", 4096, NULL, 4, NULL);
        if (pdPASS != rc)
        {
            ESP_LOGE(TAG, "Failed to create persist task");
            esp_restart();
        }
    }
}
