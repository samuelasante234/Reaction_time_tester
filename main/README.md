# ESP32-S3 Bare-Metal Reaction Game: Main Application Loop

## Overview
This file serves as the core orchestration layer for a bare-metal, two-player reaction game. It initializes the hardware peripherals and continuously evaluates a purely interrupt-driven Finite State Machine (FSM). This architecture handles game logic, hardware rendering, and player input dynamically without relying on blocking delays or bloated third-party libraries.

## Hardware & Driver Initialization
Before entering the `while(1)` super-loop, the system configures the following custom bare-metal drivers:
* **`buzzer_init()`**: Configures the hardware PWM for the piezo buzzer.
* **`gpio_init()`**: Sets up input pins for player buttons and the output pin for the trigger LED.
* **`timer1_handle()`**: Initializes the General Purpose Hardware Timer (GPTimer) for microsecond reaction tracking.
* **`st7789_init()`**: Configures the SPI bus, initializes DMA, allocates the external PSRAM framebuffer, and boots the ST7789 display controller.
* **`interrupts_init(&fsm_states)`**: Hooks hardware Interrupt Service Routines (ISRs) to the GPIO pins and passes the direct memory address of the volatile state variable.

## Architecture: The Finite State Machine (FSM)
The game logic relies on strict separation of concerns. The `main.c` loop does no decision-making; it merely routes execution to the `fsm_states_handler` based on the `volatile DRAM_ATTR States fsm_states` variable. The hardware ISRs dynamically mutate this state variable based on physical button presses.

### RTOS Decoupling & Watchdog Management
In a standard C program, an empty or tightly looping `while(1)` will crash an RTOS. To prevent the FreeRTOS Task Watchdog Timer (TWDT) from panicking, the main loop ends with a `vTaskDelay(100)`. This intentionally yields the CPU back to the FreeRTOS Idle Task to feed the watchdog, while the hardware interrupts remain fully armed and ready to instantly hijack the state.