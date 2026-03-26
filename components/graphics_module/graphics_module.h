#ifndef GRAPHICS_MODULE_H
#define GRAPHICS_MODULE_H
#include "st7789_header.h"
void draw_characters(spi_device_handle_t dev_handle, const char* user_text, int x, int y);
void clear_screen(spi_device_handle_t dev_handle);
uint32_t random_delay();

#endif