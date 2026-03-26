# ESP32-S3 High-Resolution Timer Module (`timer_module.c`)

## Overview
This module utilizes the ESP32-S3's General Purpose Timer (GPTimer) peripheral to generate an ultra-precise, hardware-backed clock that runs entirely independently of the CPU and FreeRTOS tick rate.

## Configuration & Resolution
The timer is configured to run directly off the Advanced Peripheral Bus (APB) clock. 
* **Frequency:** Set to `1,000,000 Hz` (1 MHz).
* **Resolution:** This frequency configuration means the timer mathematically increments exactly once per microsecond, providing pristine, microsecond accuracy for reaction time scoring.

## Architectural Implementation
Unlike standard stopwatch implementations that start and stop inside hardware interrupts (which risks triggering `ESP_ERR_INVALID_STATE` panics if state machines fall out of sync), this timer is designed to run continuously during the active game phase. 
* **`get_time()` Wrapper:** When a player successfully reacts, the application calls this wrapper to cleanly extract the 64-bit raw hardware count, cast it down to a manageable `uint16_t` (milliseconds), and pass it to the rendering engine for display.