# ESP32-S3 LEDC Audio Module (Passive Buzzer)

## Overview
This module provides a bare-metal, low-latency audio driver for passive buzzers using the ESP32's LED Control (LEDC) peripheral. It generates specific Pulse Width Modulation (PWM) frequencies entirely in hardware, requiring zero CPU overhead to sustain a tone.

## Silicon Architecture & Physics
Instead of using software loops to toggle GPIO pins, this module routes a dedicated hardware timer directly to the output pin. 
* **Dynamic Frequencies:** The module utilizes macros to switch between a harsh `DISQUALIFICATION_FREQ` (2000 Hz) and an energetic `WINNING_FREQ` (5000 Hz) dynamically. 
* **Resolution (`8-bit`):** Provides a duty cycle range of 0 to 255.
* **Duty Cycle (`127`):** When active, the duty cycle is locked at 127 (exactly 50%). This ensures the piezoelectric ceramic spends equal time fully expanded and fully contracted, generating the maximum physical displacement of air molecules for optimal volume.

## API Reference
* **`buzzer_init()`:** Configures the LEDC matrix. The initial duty cycle is strictly forced to `0` to prevent audio artifacts ("boot-up screech") before the software is fully initialized.
* **`buzzer_resume_winner()` / `buzzer_resume_disqualified()`:** Adjusts the hardware timer frequency, ramps the duty cycle to 50%, and updates the shadow registers to emit the required tone.
* **`buzzer_pause()`:** Instantly drops the LEDC duty cycle to `0`, flatlining the voltage to the pin and snapping the piezo crystal back to its resting state.