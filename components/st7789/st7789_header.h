#ifndef ST7789_HEADER
#define ST7789_HEADER

#define IS_REAL_ST7789 0

#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include <stdbool.h>

#define MOSI_PIN 11
#define SCK_PIN 12
#define CS_PIN 10
#define SPI_CHAN SPI2_HOST
#define DC_PIN 4
#define RES_PIN 6

#if IS_REAL_ST7789
    #define MAX_TRANSFER_SIZE 240*320*2
    #define SCREEN_WIDTH 320
#else
    #define MAX_TRANSFER_SIZE 240*240*2
    #define SCREEN_WIDTH 240
#endif
#define CLOCK_SPEED 25000000
#define DUTY_COUNT 127
#define CPOL_CPHA 3
#define TCSS 1
#define TCSH 1
#define SCREEN_HEIGHT 240
#define FONT_HEIGHT 16
#define FONT_WIDTH 8

spi_device_handle_t st7789_init();
void send_pixels(spi_device_handle_t dev_handle, uint16_t *colour, uint32_t len);
void st7789_wakeup(spi_device_handle_t dev_handle);
void st7789_fill_area(spi_device_handle_t dev_handle, uint16_t x, uint16_t y, bool is_whole_screen);

#endif