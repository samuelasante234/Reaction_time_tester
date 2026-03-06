#ifndef ST7789_HEADER
#define ST7789_HEADER

#include "driver/spi_common.h"
#include "driver/spi_master.h"

#define MOSI_PIN 11
#define SCK_PIN 12
#define CS_PIN 10
#define SPI_CHAN SPI2_HOST
#define DC_PIN 4
#define RES_PIN 5

spi_device_handle_t st7789_init();
void send_data(spi_device_handle_t dev_handle, const uint8_t* data, int len);
void send_command(spi_device_handle_t dev_handle, const uint8_t command);
void send_pixels(spi_device_handle_t dev_handle, uint16_t *colour, uint32_t len);
void st7789_wakeup(spi_device_handle_t dev_handle);
void st7789_set_window(spi_device_handle_t dev_handle, uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye);
void st7789_fill_area(spi_device_handle_t dev_handle, uint16_t x, uint16_t y, uint16_t height, uint16_t width);

#endif