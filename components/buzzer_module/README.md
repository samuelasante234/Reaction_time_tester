# ESP32-S3 LEDC Audio Module (Passive Buzzer)

## Overview
This module provides a bare-metal, low-latency audio driver for passive buzzers using the ESP32's LED Control (LEDC) peripheral. It is designed to operate within an event-driven architecture, bypassing blocking delays and utilizing hardware timers to generate Pulse Width Modulation (PWM) audio signals.

## Hardware Requirements
* **Microcontroller:** ESP32-S3(compatible with other variants)
* **Transducer:** Passive Piezoelectric Buzzer.

## Silicon Architecture & Physics
Instead of using software loops to toggle GPIO pins, this module routes a dedicated hardware timer directly to the output pin. 
* **Frequency (`4000 Hz`):** The LEDC timer is configured to oscillate at 4 kHz. This frequency is specifically chosen to match the peak resonant frequency of standard piezoelectric crystals, producing the sharpest and loudest possible sound wave.
* **Resolution (`8-bit`):** Provides a duty cycle range of 0 to 255.
* **Duty Cycle (`127`):** When active, the duty cycle is locked at 127 (exactly 50%). This ensures the piezoelectric ceramic spends equal time fully expanded and fully contracted, generating the maximum physical displacement of air molecules for optimal volume.

## API Reference

### `void buzzer_init()`
Configures the LEDC peripheral matrix.
* Initializes the LEDC Timer with a 4 kHz heartbeat.
* Routes the timer to the specified GPIO channel.
* **Safety Feature:** The initial duty cycle is strictly forced to `0` upon configuration to prevent audio artifacts ("boot-up screech") before the software is fully initialized.

### `void buzzer_resume(bool *state)`
Triggers the hardware to begin PWM generation.
* **ISR-Optimized Error Handling:** This function is designed to be called from within a hardware Interrupt Service Routine (ISR). Because standard `printf()` functions are blocking and will crash an ISR, this function uses deferred error reporting. 
* **Parameters:** * `state`: A pointer to a boolean variable. If the ESP-IDF API fails to update the hardware registers, the function silently flips this boolean to `false` and exits, allowing the main loop to handle the error reporting without halting the CPU.

### `void buzzer_pause()`
Silences the transducer.
* Instantly drops the LEDC duty cycle to `0` and updates the shadow registers, flatlining the voltage to the pin and snapping the piezo crystal back to its resting state.

## Implementation Notes for Interrupts
*When calling `buzzer_resume()` inside an ISR, monitor the system for Core Panics. If the underlying `ledc_set_duty` ESP-IDF API utilizes FreeRTOS Mutexes instead of hardware spinlocks in your specific IDF version, a deadlock may occur. In such cases, the API call should be bypassed in favor of writing the duty cycle directly to the silicon registers.*