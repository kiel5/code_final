#ifndef BOOTLOADER_COMMAND_H
#define BOOTLOADER_COMMAND_H

#include "stdint.h"

// state
typedef enum
{
    OTA_IDLE_STATE,
    OTA_SENDED_START_STATE,
    OTA_SENDED_INFOR_STATE,
    OTA_SENDED_DATA_STATE,
    OTA_SENDED_END_STATE
} OTA_STATE;
// command ID
enum
{
    OTA_CODE,
    OTA_HEADER,
    OTA_DATA,
    OTA_RESPONSE
};
//cmd start stop request(stm32)
typedef enum
{
    OTA_START,
    OTA_STOP,
    OTA_REQUEST
} OTA_CODE_NAME;
//cmd ack
typedef enum
{
    OTA_ACK,
    OTA_NACK
} OTA_RESPONSE_NAME;
//struct command send start or stop or request(stm32)
typedef struct
{
    uint8_t command_id;
    uint8_t len;
    uint8_t ota_cmd;
} __attribute__((packed)) OTACode;
// struct command send name, version
typedef struct
{
    uint8_t command_id;
    uint8_t len;
    uint8_t name[50];
    uint8_t version[10];
} __attribute__((packed)) OTAInfor;
//struct command send reponse
typedef struct
{
    uint8_t command_id;
    uint8_t len;
    uint8_t ack;
} __attribute__((packed)) OTAReponse;
//struct command send data
typedef struct
{
    uint8_t command_id;
    uint8_t len;
    uint8_t data[16];
} __attribute__((packed)) OTAData;

void bootloader_init();
void bootloader_handle(uint8_t *data);
void ota_send_code_start_stop(OTA_CODE_NAME ota_code_name);
void ota_send_reponse(OTA_RESPONSE_NAME ack);
void ota_send_infor();
void bootloader_send_data(void *data, uint8_t len);
void ota_send_data(uint8_t* data, uint8_t len);
void ota_send_code_start_stop1(OTA_CODE_NAME ota_code_name);
#endif