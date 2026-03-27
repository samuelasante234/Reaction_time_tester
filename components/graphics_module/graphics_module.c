#include "graphics_module.h"
#include "st7789_header.h"
#include "esp_heap_caps.h"
#include "stdint.h"
#include "font_module.h"
#include <string.h>
#include "esp_cache.h"
#include <stdbool.h>
#include "esp_random.h"
#define MAX_NO_OF_CHARACTERS 30

static uint16_t* convert_text_pixels(uint8_t *text, int no_of_characters);
static int get_index(int i, int num_of_chars);
void draw_characters(spi_device_handle_t dev_handle, const char* user_text, int x, int y);
void clear_screen(spi_device_handle_t dev_handle);
uint32_t random_delay();

static uint16_t* convert_text_pixels(uint8_t *text, int no_of_characters) {
    static uint16_t *Overhead = NULL;
    if(!Overhead) Overhead = (uint16_t*)heap_caps_aligned_calloc(64, 1,(uint32_t)(SCREEN_HEIGHT*SCREEN_WIDTH*2),MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
    if (!Overhead) {
        return NULL;
    }
    int k=0;
    for (int i=0; i<FONT_HEIGHT*no_of_characters; i++) {
        for (int j=FONT_WIDTH-1; j>=0;j--, k++){
            if (*(text+get_index(i, no_of_characters))&(1<<j)) *(Overhead+k) = 0x0000;
            else *(Overhead+k) =0xFFFF;
        }
    }
    esp_err_t result = esp_cache_msync((void*) Overhead,(uint32_t)(no_of_characters)*FONT_HEIGHT*FONT_WIDTH*2,ESP_CACHE_MSYNC_FLAG_DIR_C2M|ESP_CACHE_MSYNC_FLAG_TYPE_DATA);
    if (result != ESP_OK) {
        printf("Couldn't sync memory! Error: %s\n",esp_err_to_name(result));
        fflush(stdout);
        return NULL;
    }
    return Overhead;
}
static int get_index(int i, int num_of_chars) {
    int row = i/num_of_chars, column=i%num_of_chars;
    return FONT_HEIGHT*column + row;
}
void draw_characters(spi_device_handle_t dev_handle, const char* user_text, int x, int y) {
    int no_of_characters = strlen(user_text),l;
    uint8_t converted_text[16*MAX_NO_OF_CHARACTERS];
    char padded_buffer[MAX_NO_OF_CHARACTERS+1];
    padded_buffer[MAX_NO_OF_CHARACTERS] ='\0';
    int left_right_x=x-0;
    for (l=0;(left_right_x-8)>=0;l++) left_right_x-=FONT_WIDTH,padded_buffer[l]=' ';
    if (left_right_x) padded_buffer[l]=' ',l++, x+=(FONT_WIDTH-left_right_x);
    for (int k=0;k<no_of_characters && (l<MAX_NO_OF_CHARACTERS);l++,k++) padded_buffer[l]=*(user_text+k);
    left_right_x=SCREEN_WIDTH-(x +(FONT_WIDTH*no_of_characters));
    for (;(left_right_x-FONT_WIDTH)>=0;l++) left_right_x-=FONT_WIDTH,padded_buffer[l]=' ';
    for (int i=0; i<MAX_NO_OF_CHARACTERS;i++) {
        for (int k=0; k<(FONT_HEIGHT); k++) {
            converted_text[16*i+k]= ascii_8x16_font[*(padded_buffer+i)-32][k];
        }
    }
    uint16_t *ptr_to_text= convert_text_pixels(converted_text,MAX_NO_OF_CHARACTERS);
    if (!ptr_to_text) {
        printf("Couldn't reserve space!");
        fflush(stdout);
        return;
    }
    st7789_fill_area(dev_handle,0,y,false);
    send_pixels(dev_handle,ptr_to_text,FONT_HEIGHT*FONT_WIDTH*(uint32_t)MAX_NO_OF_CHARACTERS);
}
void clear_screen(spi_device_handle_t dev_handle) {
    static uint16_t *Overheader = NULL;
    if(!Overheader) Overheader = (uint16_t*)heap_caps_aligned_calloc(64, 1,(uint32_t)(SCREEN_HEIGHT*SCREEN_WIDTH*2),MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
    if (!Overheader) {
        return;
    }
    for (int i=0; i<(SCREEN_HEIGHT*SCREEN_WIDTH);i++) {
        *(Overheader +i)=0xFFFF;
    }
    esp_err_t result = esp_cache_msync((void*) Overheader,SCREEN_HEIGHT*SCREEN_WIDTH*2,ESP_CACHE_MSYNC_FLAG_DIR_C2M|ESP_CACHE_MSYNC_FLAG_TYPE_DATA);
    if (result != ESP_OK) {
        printf("Couldn't sync memory! Error: %s\n",esp_err_to_name(result));
        fflush(stdout);
        return;
    }
    st7789_fill_area(dev_handle,0,0,true);
    send_pixels(dev_handle,Overheader,SCREEN_HEIGHT*SCREEN_WIDTH);
}
uint32_t random_delay() {
    return ((esp_random() %8001)+2000);
}