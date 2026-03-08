# ESP32-S3 Bare-Metal Graphics Engine (`graphics_module.c`)

## Overview
This module contains the custom-built, bare-metal text rendering engine for the ST7789 SPI display. It is designed to bypass standard bloated graphics libraries by manually managing external PSRAM, bitmasking 1D font arrays, and mathematically interleaving pixel data to match the physical rasterization path of the display hardware. 

## Encapsulation & Scope
To ensure strict namespace protection and aggressive compiler optimization, internal algorithmic and memory-management functions (`convert_text_pixels`, `get_index`) are declared as `static`. Only the primary rendering API (`draw_characters`) is exposed to the global application loop.

## Core Functions & Architecture

### 1. The Public API: `draw_characters()`
This function serves as the entry point for the text engine. 
* **Stack VLA Allocation:** It dynamically flattens the 2D font dictionary (`ascii_8x8_font`) into a contiguous 1D C99 Variable Length Array (VLA) based on the specific characters passed in the string.
* **Hardware Windowing:** It calls `st7789_fill_area()` (which contains automated hardware boundary clamping) to open an exactly sized pixel window on the ST7789.
* **DMA Execution:** It triggers `send_pixels()` to blast the fully processed 16-bit RGB565 data over the SPI bus using Direct Memory Access (DMA).

### 2. The Memory Manager: `convert_text_pixels()`
This function handles the heavy lifting of memory allocation and color translation.
* **Persistent PSRAM:** It utilizes an internal `static` pointer initialized to `NULL`. On the first call, it uses `heap_caps_malloc(..., MALLOC_CAP_SPIRAM)` to permanently reserve a screen-sized buffer in the ESP32-S3's external PSRAM. Because the pointer is `static`, it survives between function calls, preventing memory leaks while avoiding the limited internal SRAM.
* **Bitmasking:** It iterates through the 8-bit font array, applying a bitmask (`1<<j`) to check each bit. It converts `1`s into White (`0xFFFF`) and `0`s into Black (`0x0000`).



### 3. The Interleaving Algorithm: `get_index()`
This mathematical helper function solves the hardware rasterization trap. 
Because the ST7789 display controller draws pixels left-to-right across the *entire* window rather than character-by-character, standard linear arrays result in garbled text. This function uses division and modulo arithmetic to mathematically interleave the 1D font array on the fly. It ensures the SPI DMA pipeline feeds the display exactly the pixels it expects.