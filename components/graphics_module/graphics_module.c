#include "graphics_module.h"
#include "st7789_header.h"
#include "esp_heap_caps.h"
#include "stdint.h"
#include "font_module.h"

static uint16_t* convert_text_pixels(uint8_t *text, int no_of_characters);
static int get_index(int i, int num_of_chars);
void draw_characters(spi_device_handle_t dev_handle, const char* user_text, int no_of_characters, int x, int y);

static uint16_t* convert_text_pixels(uint8_t *text, int no_of_characters) {
    static uint16_t *Overhead = NULL;
    if(!Overhead) Overhead = (uint16_t*)heap_caps_aligned_calloc(64, 1,(uint32_t)(240*320*2),MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
    if (!Overhead) {
        return NULL;
    }
    int k=0;
    for (int i=0; i<8*no_of_characters; i++) {
        for (int j=7; j>=0;j--, k++){
            if (*(text+get_index(i, no_of_characters))&(1<<j)) *(Overhead+k) = 0xFFFF;
            else *(Overhead+k) =0x0000;
        }
    }
    return Overhead;
}
static int get_index(int i, int num_of_chars) {
    int row = i/num_of_chars, column=i%num_of_chars;
    return 8*column + row;
}
void draw_characters(spi_device_handle_t dev_handle, const char* user_text, int no_of_characters, int x, int y) {
    uint8_t converted_text[8*no_of_characters];
    for (int i=0; i<no_of_characters;i++) {
        for (int k=0; k<(8); k++) {
            converted_text[8*i+k]= ascii_8x8_font[*(user_text+i)-32][k];
        }
    }
    uint16_t *ptr_to_text= convert_text_pixels(converted_text,no_of_characters);
    if (!ptr_to_text) {
        printf("Couldn't reserve space!");
        fflush(stdout);
        return;
    }
    st7789_fill_area(dev_handle,x,y,no_of_characters);
    send_pixels(dev_handle,ptr_to_text,64*(uint32_t)no_of_characters);
}