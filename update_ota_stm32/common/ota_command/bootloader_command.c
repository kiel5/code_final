#include "bootloader_command.h"
#include "min.h"
#include "string.h"
#include "download_file.h"
#include "esp_log.h"
#include "driver/uart.h"
static const char *TAG = "boot_loader";
#define MIN_PORT 0
#define MIN_ID 1
struct min_context min_cxt;

OTA_STATE ota_state;
//-------------init min context-----------
extern char file_hex[];
uint16_t index_file_hex = 0;
void bootloader_init()
{
    min_init_context(&min_cxt, MIN_PORT);
    ota_state = OTA_IDLE_STATE;
}
//-------------function send data----------
void bootloader_send_data(void *data, uint8_t len)
{
    min_send_frame(&min_cxt, MIN_ID, (uint8_t *)data, len);
}
void min_application_handler(uint8_t min_id, uint8_t const *min_payload,
                             uint8_t len_payload, uint8_t port)
{
    ESP_LOGI(TAG, "dcmmm");
    bootloader_send_data("kienkm59",8);
    
    // trao đổi dữ liệu với stm32
    switch (ota_state)
    {

    case OTA_IDLE_STATE:
        // min_payload: 0x00,0x01,0x02
        OTACode *otacode = (OTACode *)min_payload;
        if (otacode->command_id == OTA_CODE && otacode->ota_cmd == OTA_REQUEST)
        {
            index_file_hex = 0;
            ota_state = OTA_SENDED_START_STATE;
            ota_send_code_start_stop(OTA_START);
        }
        else
        {
            ota_state = OTA_IDLE_STATE;
        }

        break;
    case OTA_SENDED_START_STATE:
        // neu nhan ack thi gui infor
        OTAReponse *otareponse = (OTAReponse *)min_payload;
        if (otareponse->command_id == OTA_RESPONSE && otareponse->ack == OTA_ACK)
        {
            ota_state = OTA_SENDED_INFOR_STATE;
            ota_send_infor();
        }
        else
        {
            ota_state = OTA_IDLE_STATE;
        }
        break;
    case OTA_SENDED_INFOR_STATE:
    {
        OTAReponse *otareponse = (OTAReponse *)min_payload;
        if (otareponse->command_id == OTA_RESPONSE && otareponse->ack == OTA_ACK)
        {
            uint8_t hex_data[21];
            char *token = strtok(file_hex, "\n");
            convert_string_to_array_hex(token, hex_data);
            check_some(hex_data, hex_data[0] + 5);
            swap_4_byte(hex_data + 4, hex_data[0]);
            ota_send_data(hex_data + 4, hex_data[0]);
            ota_state = OTA_SENDED_DATA_STATE;
        }
        else
        {
            ota_state = OTA_IDLE_STATE;
        }
    }
    break;
    case OTA_SENDED_DATA_STATE:
    {
        OTAReponse *otareponse = (OTAReponse *)min_payload;
        if (otareponse->command_id == OTA_RESPONSE && otareponse->ack == OTA_ACK)
        {
            char *token = strtok(NULL, "\n");
            if (token != NULL)
            {
                uint8_t hex_data[21];
                convert_string_to_array_hex(token, hex_data);
                if (hex_data[3] == 0x00)
                {
                    if (check_some(hex_data, hex_data[0] + 5) == status_error)
                    {
                        ota_state = OTA_SENDED_END_STATE;
                        return;
                    }
                    swap_4_byte(hex_data + 4, hex_data[0]);
                    ota_send_data(hex_data + 4, hex_data[0]);
                }
                else
                {
                    ota_send_code_start_stop(OTA_STOP);
                    ota_state = OTA_SENDED_END_STATE;
                }
            }
        }
    }

        break;
        case OTA_SENDED_END_STATE:
        {
            OTAReponse *otareponse = (OTAReponse *)min_payload;
            if (otareponse->command_id == OTA_RESPONSE && otareponse->ack == OTA_ACK)
            {
                // do something
                ota_state = OTA_IDLE_STATE;
            }
        }
        break;

        default:
            break;
    }
}
void bootloader_handle(uint8_t *data)
{

    min_send_frame(&min_cxt, MIN_ID, (uint8_t*)"kienkmkm", 8);
    min_poll(&min_cxt, data, 1);
}
void ota_send_code_start_stop(OTA_CODE_NAME ota_code_name)
{
    OTACode otacode;
    otacode.command_id = OTA_CODE;
    otacode.len = 1;
    otacode.ota_cmd = ota_code_name;
    bootloader_send_data(&otacode, sizeof(otacode));
}
void ota_send_reponse(OTA_RESPONSE_NAME ack)
{
    OTAReponse otareponse;
    otareponse.command_id = OTA_RESPONSE;
    otareponse.len = 1;
    otareponse.ack = ack;
    bootloader_send_data(&otareponse, sizeof(otareponse));
}
void ota_send_infor()
{
    OTAInfor otainfor;
    otainfor.command_id = OTA_HEADER;
    otainfor.len = sizeof(otainfor.name) + sizeof(otainfor.version);
    strcpy((char *)&otainfor.name, "update");
    strcpy((char *)&otainfor.version, "1.0.0");
    bootloader_send_data(&otainfor, sizeof(otainfor));
}
void ota_send_data(uint8_t *data, uint8_t len)
{
    OTAData otadata;
    otadata.command_id = OTA_DATA;
    otadata.len = sizeof(otadata);
    memcpy(otadata.data, data, len);
    bootloader_send_data(&otadata, sizeof(otadata));
}




void ota_send_code_start_stop1(OTA_CODE_NAME ota_code_name)
{
    OTACode otacode;
    otacode.command_id = OTA_CODE;
    otacode.len = 1;
    otacode.ota_cmd = ota_code_name;
    uart_write_bytes(UART_NUM_0,&otacode, sizeof(otacode));
}