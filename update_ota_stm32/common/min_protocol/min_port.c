#include "min.h"
#include "stdint.h"
#include "freertos/FreeRTOS.h"
#include "driver/uart.h"


void min_tx_start(uint8_t port) {}
void min_tx_finished(uint8_t port) {}

void min_tx_byte(uint8_t port, uint8_t byte) // hàm tự định nghĩa mỗi chip: gửi uart 1 byte
{
  uart_write_bytes(UART_NUM_0, &byte, 1);
}
uint16_t min_tx_space(uint8_t port)
{
  return 512;
}
uint32_t min_time_ms(void)
{
  return xTaskGetTickCount();
}
