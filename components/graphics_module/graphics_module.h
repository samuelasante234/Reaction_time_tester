#ifndef GRAPHICS_MODULE_H
#define GRAPHICS_MODULE_H
#include "st7789_header.h"
void draw_characters(spi_device_handle_t dev_handle, const char* user_text, int no_of_characters, int x, int y);

#endif