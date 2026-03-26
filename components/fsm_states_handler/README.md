# ESP32-S3 Finite State Machine Handler (`fsm_states_handler.c`)

## Overview
This module isolates the logical states and visual transitions of the game from the main loop. It dictates exactly what is rendered to the ST7789 screen, how the buzzer's driver responds, and when the hardware timers are engaged.

## Macro-Driven Architecture
To ensure clean, maintainable code, all blocking delays are defined via macros (e.g., `DELAY_1_S`, `DELAY_2_S`). This eliminates magic numbers and makes tuning the game's UX simple and centralized.

## The Polling Delay Breakout (Asynchronous UI)
Standard RTOS delays (`vTaskDelay`) suspend the current task completely. If a player jumps the gun during the random 2-to-10 second tension phase, a standard 5-second delay would prevent the screen from updating until the sleep finished.

To solve this, the `trigger_state` implements a bare-metal **Polling Delay Breakout**:
```c
while (track < delay) {
    if (*state != TRIGGER_STATE) return; 
    track++;
    vTaskDelay(1);
}