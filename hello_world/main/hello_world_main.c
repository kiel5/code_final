
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "output_iot.h"
#include "input_iot.h"

TimerHandle_t xTimers[ 2 ];
EventGroupHandle_t xEventGroup;

#define BIT_EVENT_BUTTON_PRESS (1<<0)
#define BIT_EVENT_UART_BECV    (1<<1)
void vTask1( void * pvParameters )
{   
    for( ;; )
    {
       EventBits_t  uxBits = xEventGroupWaitBits(
            xEventGroup,   /* The event group being tested. */
            BIT_EVENT_BUTTON_PRESS | BIT_EVENT_UART_BECV, /* The bits within the event group to wait for. */
            pdTRUE,        /* BIT_0 & BIT_4 should be cleared before returning. */
            pdFALSE,       /* Don't wait for both bits, either bit will do. */
            portMAX_DELAY );/* Wait a maximum of 100ms for either bit to be set. */
        if(uxBits & BIT_EVENT_BUTTON_PRESS)
        {
          printf("button press\n");
          output_io_toggle(2);
        }
        if(uxBits & BIT_EVENT_UART_BECV)
        {
          printf("uart data");
          // .... 
        }
    }
}
void vTimerCallback( TimerHandle_t xTimer )
 {
    configASSERT( xTimer );
    int ulCount = ( uint32_t ) pvTimerGetTimerID( xTimer );
    if(ulCount == 0)
    {
      output_io_toggle(2);
    }
    if(ulCount == 1)
    {
      printf("hello\n");
    }
 }
void button_callback(int pin)
{
  if(pin == GPIO_NUM_0)
  {
    BaseType_t xEventGroupSetBitsFromISR;
    xEventGroupSetBitsFromISR(xEventGroup,BIT_EVENT_BUTTON_PRESS,&xEventGroupSetBitsFromISR);

  }
}
void app_main(void)
{
  xTimers[ 0 ] = xTimerCreate("TimerBlink",pdMS_TO_TICKS(500), pdTRUE, (void*)0,vTimerCallback);     
  xTimers[ 1 ] = xTimerCreate("TimerPrint",pdMS_TO_TICKS(1000), pdTRUE, (void*)1,vTimerCallback);
  output_io_create(2);
  input_io_create(0,HI_TO_LO);
  input_set_callback(button_callback);
  // xTimerStart(xTimers[0],0);
  // xTimerStart(xTimers[1],0);
  xEventGroup = xEventGroupCreate();
  xTaskCreate(
                    vTask1,       /* Function that implements the task. */
                    "vTask1",          /* Text name for the task. */
                    1024 *2,      /* Stack size in words, not bytes. */
                    NULL,    /* Parameter passed into the task. */
                    4,/* Priority at which the task is created. */
                    NULL );      /* Used to pass out the created task's handle. */

}
