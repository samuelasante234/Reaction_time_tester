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
spi_transaction_t spi_data_bus_init();
void send_data(spi_device_handle_t dev_handle, const uint8_t* data, int len);
void send_command(spi_device_handle_t dev_handle, const uint8_t command);
void st7789_wakeup(spi_device_handle_t dev_handle);

#endif