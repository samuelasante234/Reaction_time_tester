#include "driver/gpio.h"
#include "st7789_header.h"
#include "soc/gpio_reg.h"
#include "esp_rom_sys.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"

static void IRAM_ATTR ISR(void *arg);
spi_device_handle_t st7789_init();
void send_data(spi_device_handle_t dev_handle, const uint8_t* data, int len);
void send_command(spi_device_handle_t dev_handle, const uint8_t command);
void st7789_wakeup(spi_device_handle_t dev_handle);
void st7789_set_window(spi_device_handle_t dev_handle, uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye);
void st7789_fill_area(spi_device_handle_t dev_handle, uint16_t x, uint16_t y, uint16_t height, uint16_t width, uint16_t colour);

spi_device_handle_t st7789_init() {
    spi_bus_config_t bus_conf = {
        .mosi_io_num = MOSI_PIN,
        .miso_io_num=-1,
        .sclk_io_num=SCK_PIN,
        .quadhd_io_num=-1,
        .quadwp_io_num=-1,
        .intr_flags=0,
    };
    esp_err_t output = spi_bus_initialize(SPI_CHAN, &bus_conf, SPI_DMA_DISABLED);
    if (output!=ESP_OK) {
        printf("Couldn't initialise bus. Error: %s\n", esp_err_to_name(output));
        fflush(stdout);
        return NULL;
    }
    spi_device_interface_config_t spi_conf={
        .clock_source=SPI_CLK_SRC_APB,
        .clock_speed_hz=200000,
        .duty_cycle_pos=127,
        .mode=0,
        .command_bits=0,
        .address_bits=0,
        .dummy_bits=0,
        .queue_size=1,
        .spics_io_num=CS_PIN,
        .cs_ena_pretrans=1,
        .cs_ena_posttrans=1,
        .pre_cb=(transaction_cb_t) ISR,
    };
    spi_device_handle_t dev_handle;
    output= spi_bus_add_device(SPI_CHAN, &spi_conf, &dev_handle);
    if (output != ESP_OK) {
        printf("Couldn't add device! Error : %s\n", esp_err_to_name(output));
        fflush(stdout);
        return NULL;
    }
    return dev_handle;
}

static void IRAM_ATTR ISR(void *arg)  {
    spi_transaction_t* flag = (spi_transaction_t *) arg;
    if (flag->user) {
        volatile uint32_t *Overhead = (volatile uint32_t *)GPIO_OUT_W1TS_REG;
        *Overhead = 1<<DC_PIN;
    }
    else {
        volatile uint32_t *Overhead = (volatile uint32_t *)GPIO_OUT_W1TC_REG;
        *Overhead = 1<<DC_PIN;
    }
}
void send_data(spi_device_handle_t dev_handle, const uint8_t* data, int len_bytes) {
    spi_transaction_t transact_t = {0};
    transact_t.length=len_bytes*8; transact_t.tx_buffer=data; transact_t.user=(void*)1;
    esp_err_t result = spi_device_polling_transmit(dev_handle,&transact_t);
    if (result != ESP_OK) {
        printf("Couldn't send data! Error: %s", esp_err_to_name(result));
        fflush(stdout);
        return;
    }
}
void send_command(spi_device_handle_t dev_handle, const uint8_t command) {
    spi_transaction_t transact_t ={0};
    transact_t.length=8; transact_t.tx_buffer=&command; transact_t.user=(void*)0;
    esp_err_t result = spi_device_polling_transmit(dev_handle, &transact_t);
    if (result != ESP_OK) {
        printf("Couldn't send command! Error: %s\n", esp_err_to_name(result));
        fflush(stdout);
        return;
    }
}
void st7789_wakeup(spi_device_handle_t dev_handle) {
    volatile uint32_t *Overhead_set = (volatile uint32_t *) GPIO_OUT_W1TS_REG;
    volatile uint32_t *Overhead_clear = (volatile uint32_t *) GPIO_OUT_W1TC_REG;
    *Overhead_set = 1<<RES_PIN;
    gpio_set_direction(RES_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(DC_PIN, GPIO_MODE_OUTPUT);
    *Overhead_clear = 1<<RES_PIN;
    esp_rom_delay_us(14);
    *Overhead_set=1<<RES_PIN;
    vTaskDelay(120);
    send_command(dev_handle, 0x01); //software reset
    vTaskDelay(120);
    send_command(dev_handle, 0x11); //sleep out
    vTaskDelay(120);
    send_command(dev_handle, 0x3A); //color mode
    uint8_t temp = 0x55;
    send_data(dev_handle,&temp, 1);
    send_command(dev_handle, 0x36); //memory address control
    temp = 0x00;
    send_data(dev_handle,&temp, 1);
    send_command(dev_handle,0x29); //display on
}
void st7789_set_window(spi_device_handle_t dev_handle, uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye) {
    send_command(dev_handle, 0x2A);
    uint8_t temp[4]={xs>>8, xs, xe>>8, xe};
    send_data(dev_handle,temp,4);
    send_command(dev_handle, 0x2B);
    temp[0]=ys>>8, temp[1]=ys, temp[2]=ye>>8, temp[3]=ye;
    send_data(dev_handle,temp,4);
    send_command(dev_handle, 0x2C);
}
void st7789_fill_area(spi_device_handle_t dev_handle, uint16_t x, uint16_t y, uint16_t height, uint16_t width, uint16_t colour) {
    if (x>=320) x=319;
    if (y>=240) y=239;
    if ((x+width)>320) {
        width=320, width-=x;
    }
    if ((y+height) >240) {
        height=240, height -=y;
    }
    st7789_set_window(dev_handle, x,x + width-1, y, y + height-1);
    uint32_t tot_pixels = width*height;
    uint8_t rgb_16[2]={colour>>8, colour};
    for (uint32_t i=0; i<tot_pixels; i++) send_data(dev_handle,rgb_16, 2);
}