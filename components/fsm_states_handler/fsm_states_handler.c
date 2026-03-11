#include <stdio.h>
#include "st7789_header.h"
#include "timer_module.h"
#include "buzzer_module.h"
#include "gpio_module.h"
#include "graphics_module.h"
#include <stdint.h>
#include "fsm_states_handler.h"

void idle_state(spi_device_handle_t dev_handle,States *state);
void welcome_state(spi_device_handle_t dev_handle,States *state);
void trigger_state(spi_device_handle_t dev_handle, States *state, int round);

void idle_state(spi_device_handle_t dev_handle, States *state) {
    disable_interrupt_1();
    disable_interrupt_2();
    st7789_wakeup(dev_handle);
    *state=WELCOME_STATE;
}
void welcome_state(spi_device_handle_t dev_handle,States *state) {
    draw_characters(dev_handle,"NEW GAME!",50, 10);
    draw_characters(dev_handle, "Rules of the game!",50,10);
    enable_interrupt_1();
    enable_interrupt_2();
    *state=TRIGGER_STATE;
}
void trigger_state(spi_device_handle_t dev_handle, States *state, int round) {
    buzzer_pause();
    int dummy=round+1;
    char *text = "Round "+dummy;
    draw_characters(dev_handle, "Game start!",100, 10);
    gpio_set_level(LED_PIN,1);
    *state=NOTHING_STATE;
}