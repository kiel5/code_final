#include "i2c_oled.h"

#include "ssd1306.h"
#include "font8x8_basic.h"

#define CONFIG_SDA_GPIO 21
#define CONFIG_SCL_GPIO 22
#define CONFIG_RESET_GPIO 0

SSD1306_t dev;
void oled_init(void)
{
    i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
    ssd1306_init(&dev, 128, 32);
}

void oled_display_text(char *data,int row,int length)
{
    ssd1306_clear_screen(&dev, false);
    ssd1306_contrast(&dev, 0xff);
    ssd1306_display_text(&dev, row, data, strlen(data), false);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
}
void oled_display_horizental(char *data, int row, int length)
{
	ssd1306_clear_screen(&dev, false);  // xoá màn hinh
	ssd1306_contrast(&dev, 0xff);       // đặt chế độ tương phản
	ssd1306_display_text(&dev, row, data, length, false);  // không đảo ngược
	//ssd1306_hardware_scroll(&dev, SCROLL_RIGHT);
	//vTaskDelay(5000 / portTICK_PERIOD_MS);
	ssd1306_hardware_scroll(&dev, SCROLL_LEFT);
	//vTaskDelay(5000 / portTICK_PERIOD_MS);
	// ssd1306_hardware_scroll(&dev, SCROLL_STOP);
}
