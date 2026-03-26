# ESP32-S3 Bare-Metal Graphics Engine (`graphics_module.c`)

## Overview
This module contains the custom-built, bare-metal text rendering engine for the ST7789. It bypasses bloated graphics libraries by manually managing external PSRAM, bitmasking 1D font arrays, and mathematically interleaving pixel data to match the physical rasterization path of the display hardware.

## PSRAM Cache Coherency & Allocation
To prevent internal SRAM exhaustion, the engine permanently allocates the 115KB framebuffer in the ESP32-S3's external SPI RAM using `heap_caps_aligned_calloc(..., MALLOC_CAP_SPIRAM)`. 
Because the CPU writes to this memory but the external DMA hardware reads from it, the engine executes `esp_cache_msync()` to explicitly flush the CPU cache lines to physical memory before triggering an SPI transfer.

## Core Functions
### `draw_characters()`
* Dynamically flattens the 2D font dictionary into a contiguous 1D array.
* Pads the text dynamically based on a `MAX_NO_OF_CHARACTERS 30` calculation to ensure clean row resets on a 240x240 screen without trailing artifacts.
* Triggers the bitmasking engine and fires the DMA transmission.

### `convert_text_pixels()`
* Iterates through the 8-bit font array, applying a bitmask (`1<<j`). 
* Converts `1`s into White (`0xFFFF`) and `0`s into Black (`0x0000`).

### `get_index()`
* Because the ST7789 display controller draws pixels left-to-right across the *entire* window rather than character-by-character, standard linear arrays result in garbled text. This helper function uses modulo arithmetic to mathematically interleave the 16-row font array.