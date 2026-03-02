#include <stdio.h>
#include "st7789_header.h"

void app_main(void)
{
    spi_device_handle_t st7789_handle= st7789_init();
    st7789_wakeup(st7789_handle);
    st7789_fill_area(st7789_handle,0,0,240,320, 0xF800); //red
    vTaskDelay(10);
    st7789_fill_area(st7789_handle,0,0,240,320,0x07E0); //green
    vTaskDelay(10);
    st7789_fill_area(st7789_handle,0,0,240,320,0x001F); //blue
}