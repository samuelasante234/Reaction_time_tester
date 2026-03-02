# ST7789 ESP-IDF SPI Driver

A hardware-accelerated, highly encapsulated SPI driver for the ST7789 display controller, built natively for the ESP-IDF FreeRTOS environment. 

## Architecture overview
This component acts as a "Black Box" transport and initialization layer. It hides the complexities of the ESP32 `spi_transaction_t` structs and FreeRTOS bus arbitration from the main application layer. It utilizes `spi_device_polling_transmit` for memory-safe, ultra-fast initialization commands.

## Features Currently Implemented
* **Hardware SPI Initialization:** Configures the ESP32 SPI Master and registers the display as a device on the bus.
* **Encapsulated Data/Command Wrappers:** Safely dynamically allocates transaction structs and handles payload transmission.
* **Hardware Interrupt Handling:** Uses the SPI transaction `.user` flag to automatically toggle the Data/Command (D/C) GPIO pin via an ISR, keeping the CPU free.
* **Boot Sequence:** Implements the manufacturer-specified hardware reset pulse (us precision) and the software sleep-out/color-mode initialization sequence (ms precision).

## API Reference

### `spi_device_handle_t st7789_init()`
Initializes the SPI bus and adds the ST7789 device. Returns the SPI handle required by all other driver functions. 
* *Note: Configured for 25MHz APB clock source.*

### `void st7789_wakeup(spi_device_handle_t dev_handle)`
Executes the strict hardware reset and software initialization sequence.
* Toggles the `RES` pin with a 14µs pulse.
* Sends `SWRESET` (0x01) and `SLPOUT` (0x11) with required FreeRTOS 120ms blocking delays.
* Configures display for 16-bit RGB565 color format.

### `void send_command(spi_device_handle_t dev_handle, const uint8_t command)`
Transmits a single 8-bit command to the display. Automatically pulls the D/C pin LOW via the pre-transmission callback.

### `void send_data(spi_device_handle_t dev_handle, const uint8_t* data, int len_bytes)`
Transmits an array of data bytes to the display. Automatically pulls the D/C pin HIGH via the pre-transmission callback.

### `void st7789_set_window(spi_device_handle_t dev_handle, uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye)`
Receives the start and end coordinates of the area to paint. Transmits the coordinates to `CASET` and `RASET` respectively to set up the window. Uses `RAMWR` to start painting.

### `void st7789_fill_area(spi_device_handle_t dev_handle, uint16_t x, uint16_t y, uint16_t height, uint16_t width, uint16_t colour)`
Simplifies the math by just asking for start and region to draw. Calculates the start and end coordinates and passes to `st7789_set_window`. Paints every pixel in the defined region with `colour`.
