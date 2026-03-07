# ESP32-S3 Bare-Metal Reaction Game: Main Application Loop ()

## Overview
This file serves as the core orchestration layer for a bare-metal, two-player reaction game. It initializes the hardware peripherals and continuously evaluates a purely interrupt-driven Finite State Machine (FSM). This architecture handles game logic, hardware rendering, and player input dynamically without relying on blocking delays or bloated third-party libraries.

## Hardware & Driver Initialization
Before entering the `while(1)` super-loop, the system configures the following custom bare-metal drivers:
* **`buzzer_init()`**: Configures the hardware PWM for the piezo buzzer.
* **`gpio_init()`**: Sets up input pins for player buttons and the output pin for the trigger LED.
* **`timer1_handle()`**: Initializes the General Purpose Hardware Timer (GPTimer) for exact, sub-millisecond reaction tracking.
* **`st7789_init()`**: Configures the SPI bus, initializes DMA, allocates the external PSRAM framebuffer, and boots the ST7789 display controller.
* **`interrupts_init(&fsm_states)`**: Hooks hardware Interrupt Service Routines (ISRs) to the GPIO pins and passes the direct memory address of the volatile state variable.

## Architecture: The Finite State Machine (FSM)



The game logic relies on strict separation of concerns. The `main.c` loop does no decision-making; it merely reacts to the `volatile DRAM_ATTR States fsm_states` variable. The hardware ISRs dynamically mutate this state variable based on physical button presses.

### Defined States:
* **`IDLE_STATE`**: Wakes up the ST7789 display controller and prepares the system.
* **`WELCOME_STATE`**: Renders the title screen using the custom, bitwise-interleaved `draw_characters` graphics engine.
* **`TRIGGER_STATE`**: Asserts the LED pin HIGH to visually signal the start of the reaction phase.
* **`DISQUALIFIED_1_STATE` / `DISQUALIFIED_2_STATE`**: Triggered by the ISR if a player presses their button *before* the LED turns on. Renders the fault screen and clears hardware outputs.
* **`WINNER_1_STATE` / `WINNER_2_STATE`**: Triggered by the ISR if a player successfully reacts first *after* the LED turns on. Halts the buzzer and renders the victory screen.
* **`NOTHING_STATE`**: An intentional idle block. Currently implements dummy mathematics (`i %= 3`) to keep the CPU active. This prevents the FreeRTOS Task Watchdog Timer (TWDT) from panicking while the system waits for hardware interrupts to hijack the state.