# ESP32-S3 Hardware Interrupt & GPIO Manager (`gpio_module.c`)

## Overview
This module handles the physical layer of the reaction game. It configures the General Purpose Input/Output (GPIO) pins and manages the time-critical Interrupt Service Routines (ISRs). By executing hardware-level logic, it completely decouples the game's physical inputs from the main application super-loop.

## Hardware Configuration (`gpio_init`)
* **Pull-Down Resistors:** All input pins are configured with internal pull-down resistors (`pull_down_en = 1`) to prevent floating inputs and false triggers.
* **Dual-Mode Output:** The `LED_PIN` is configured as `GPIO_MODE_INPUT_OUTPUT`. This allows the application to drive the LED high/low while simultaneously allowing the ISRs to read its current hardware state as a logic check.
* **Positive Edge Triggering:** Button interrupts are configured to fire strictly on the `GPIO_INTR_POSEDGE` (the exact microsecond the signal transitions from LOW to HIGH).

## Interrupt Architecture (`interrupts_init`)
The ISR service is installed with the `ESP_INTR_FLAG_IRAM` flag. This is a critical RTOS optimization that forces the interrupt handlers to reside in the ESP32-S3's internal fast RAM rather than flash memory. This ensures microsecond latency and prevents fatal system crashes if an interrupt fires while the flash cache is disabled.



## The ISR Handlers (`IRS_BUTTON_1` & `IRS_BUTTON_2`)
To enforce security and encapsulation, the handlers are declared `static` and kept hidden from the global scope. They are decorated with `IRAM_ATTR` to satisfy the memory allocation flags.

### Logic Flow & Concurrency Control:
1. **The Hardware Mutex:** The very first instruction in the ISR disables the opponent's interrupt pin (`gpio_intr_disable`). This absolutely prevents race conditions if both players press their buttons nearly simultaneously.
2. **State Context:** The ISR casts the generic `void *arg` pointer back into the `volatile States` pointer, allowing direct, thread-safe mutation of the FSM state.
3. **The Game Logic:** * It reads the physical electrical state of the `LED_PIN`.
   * **If HIGH:** The reaction phase has started. The player wins. The FSM is updated to `WINNER_STATE`, and the hardware buzzer is triggered.
   * **If LOW:** The tension phase is still active. The player jumped the gun. The FSM is updated to `DISQUALIFIED_STATE`.
4. **Cleanup:** The opponent's interrupt is re-enabled before exiting the ISR to prepare the hardware for the next round.