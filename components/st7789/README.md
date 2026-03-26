# ST7789 ESP-IDF SPI Driver & DMA Engine (`st7789_header.c` / `st7789_header.h`)

## Overview
This module provides a hardware-accelerated, highly encapsulated bare-metal SPI driver for the ST7789 display controller. Designed specifically for the ESP32-S3 within the ESP-IDF FreeRTOS environment, it completely bypasses bloated third-party graphics libraries.

It acts as a "Black Box" transport layer, utilizing polling for safe initialization commands and heavily optimized Direct Memory Access (DMA) burst transmissions for pushing pixels, freeing up the CPU during massive screen renders.

## Core Architectural Features

### 1. The Silicon Wall: DMA Chunking Engine
The ESP32-S3 contains an 18-bit maximum transaction length register for SPI. This means the hardware physically cannot send more than 32,767 bytes (~32KB) of data in a single continuous burst. However, a full 240x240 16-bit color frame requires 115,200 bytes. 

To bypass this physical silicon limit without crashing the driver, `send_pixels()` implements a mathematical **DMA Chunking Engine**:
* It calculates a safe payload limit (`max_pixels = 16000`, which equals 32,000 bytes).
* It dynamically slices the framebuffer and increments the memory pointer (`tx_buffer = head + pixels_sent`).
* It fires consecutive SPI transmissions in a `while` loop, allowing the ST7789 to seamlessly stitch the chunks together on the glass.

### 2. Full-Duplex Ghosting Mitigation
Because full-duplex SPI acts as a continuous circular shift register, the ESP-IDF drivers automatically mutate the `rxlength` of transaction structs, assuming you want to receive as much as you transmit. To prevent `ESP_ERR_INVALID_ARG` crashes during the final uneven chunk of a frame render, the driver explicitly scrubs the struct by setting `transact_t.rxlength = 0` before every single burst.

### 3. Zero-Overhead D/C Pin Toggling
The ST7789 requires the Data/Command (D/C) pin to toggle depending on the payload. Instead of wasting CPU cycles manually toggling the GPIO, this driver uses a pre-transmission callback (`.pre_cb`). 
When a transaction begins, the SPI peripheral fires an interrupt (`IRAM_ATTR ISR`). This ISR bypasses the ESP-IDF completely and writes a bitmask directly to the ESP32-S3's raw silicon registers (`GPIO_OUT_W1TS_REG` and `GPIO_OUT_W1TC_REG`) for instant, zero-latency toggling.

### 4. Hardware-Agnostic Compilation
The header file includes a conditional compilation block (`#if IS_REAL_ST7789`). By simply toggling this single macro between `0` and `1`, the C preprocessor automatically recalculates the `MAX_TRANSFER_SIZE` and `SCREEN_WIDTH` to swap the entire graphics engine between a 240x240 and 320x240 physical panel before the code is even compiled.

## API Reference

### Initialization & Setup
* **`spi_device_handle_t st7789_init()`**
  Initializes the SPI bus and registers the display. Configured for a 25MHz APB clock source with `SPI_DMA_CH_AUTO` to let the ESP-IDF automatically allocate the best DMA channel.
* **`void st7789_wakeup(spi_device_handle_t dev_handle)`**
  Executes the strict hardware reset and software initialization sequence. It toggles the `RES` pin with a 14µs pulse, followed by `SWRESET` (0x01) and `SLPOUT` (0x11) commands with the manufacturer-required 120ms blocking delays, and configures the panel for 16-bit RGB565 color format.

### Transmission Wrappers
* **`void send_command(spi_device_handle_t dev_handle, const uint8_t command)`**
  Transmits a single 8-bit command to the display. Sets the `.user` flag to `0` to pull the D/C pin LOW.
* **`void send_data(spi_device_handle_t dev_handle, const uint8_t* data, int len_bytes)`**
  Transmits an array of configuration data bytes to the display. Sets the `.user` flag to `1` to pull the D/C pin HIGH.

### Rendering
* **`void st7789_set_window(spi_device_handle_t dev_handle, uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye)`**
  Receives the start and end bounding box coordinates. Transmits the coordinates to `CASET` (Column Address Set) and `RASET` (Row Address Set) to map the hardware drawing window, then sends the `RAMWR` (Memory Write) command to prepare the controller for incoming pixel data.
* **`void st7789_fill_area(spi_device_handle_t dev_handle, uint16_t x, uint16_t y, bool is_whole_screen)`**
  A high-level wrapper that calculates the bounds of a drawing region (preventing out-of-bounds rendering) and automatically passes the safe coordinates down to `st7789_set_window`.
* **`void send_pixels(spi_device_handle_t dev_handle, uint16_t *colour, uint32_t len)`**
  The primary DMA wrapper. It uses `esp_cache_msync` to explicitly flush the CPU cache lines to the external PSRAM buffer, then chunks the 16-bit color array over the SPI bus.