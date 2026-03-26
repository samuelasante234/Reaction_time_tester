# ESP32-S3 Hardware Interrupt & Context Manager (`gpio_module.c`)

## Overview
This module handles the physical layer of the reaction game. It configures the GPIO pins and manages the time-critical Interrupt Service Routines (ISRs). By executing hardware-level logic, it completely decouples the game's physical inputs from the FreeRTOS scheduler.

## The Timestamp Architecture (Microsecond Debouncing)
Mechanical buttons inherently "bounce," creating hundreds of false electrical signals in a single press. Instead of using blocking `vTaskDelay` debouncing (which crashes ISRs), this module uses the ESP32's internal 64-bit CPU cycle counter: `esp_timer_get_time()`.
* It captures the exact microsecond the physical metal makes contact.
* It compares the `final` time against a `static initial` time.
* If the difference is less than `MAX_DEBOUNCE_TIME` (20,000 µs), it silently drops the ghost interrupt.

## The ISR Handlers (`IRS_BUTTON_1` & `IRS_BUTTON_2`)
To enforce security and optimize speed, the handlers are declared `static` and forced into `IRAM_ATTR`. This forces the compiled code into the chip's internal fast RAM, ensuring microsecond execution latency without waiting for external flash memory to wake up.

### Logic Flow & Concurrency Guardrails:
1. **The Mutex:** The very first instruction disables the opponent's interrupt pin to mathematically prevent a tie condition.
2. **State Context Passing:** The ISR casts the generic `void *arg` pointer back into the global `volatile States` pointer to safely mutate the game flow.
3. **The Race Condition Guardrails:** Before applying any game logic, the ISR checks a series of `if (*current_state == ...)` conditions. This permanently locks out losing players from accidentally overwriting a `WINNER` or `DISQUALIFIED` state if they press their button a few milliseconds late.
4. **The Trigger Logic:** * If `LED_PIN` is HIGH: Player wins.
   * If `LED_PIN` is LOW: Player jumped the gun and is disqualified.