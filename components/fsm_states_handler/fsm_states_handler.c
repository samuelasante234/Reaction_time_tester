#include <stdio.h>
#include "st7789_header.h"
#include "timer_module.h"
#include "buzzer_module.h"
#include "gpio_module.h"
#include "graphics_module.h"
#include <stdint.h>
#include <string.h>
#include "fsm_states_handler.h"

void idle_state(spi_device_handle_t dev_handle,States *state);
void welcome_state(spi_device_handle_t dev_handle,States *state);
void trigger_state(spi_device_handle_t dev_handle, States *state, int round);
void disqualified_1_state(spi_device_handle_t dev_handle, States *state);
void disqualified_2_state(spi_device_handle_t dev_handle, States *state);
void winner_1_state(spi_device_handle_t dev_handle);
void winner_2_state(spi_device_handle_t dev_handle);
void game_end_state(spi_device_handle_t dev_handle);

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
    char text[20];
    snprintf(text,sizeof(text), "Round %d", dummy);
    draw_characters(dev_handle,text,100,10);
    draw_characters(dev_handle, "Game start!",100, 10);
    vTaskDelay(500);
    gpio_set_level(LED_PIN,1);
    *state=NOTHING_STATE;
}
void disqualified_1_state(spi_device_handle_t dev_handle, States *state) {
    disable_interrupt_1();
    disable_interrupt_2();
    buzzer_resume_disqualified();
    gpio_set_level(LED_PIN,0);
    draw_characters(dev_handle, "Player 1 disqualified",100,10);
    vTaskDelay(2000);
    buzzer_pause();
    *state=GAME_END_STATE;
}
void disqualified_2_state(spi_device_handle_t dev_handle, States *state) {
    disable_interrupt_1();
    disable_interrupt_2();
    buzzer_resume_disqualified();
    gpio_set_level(LED_PIN,0);
    draw_characters(dev_handle, "Player 2 disqualified",100,10);
    vTaskDelay(2000);
    buzzer_pause();
    *state=GAME_END_STATE;
}
void winner_1_state(spi_device_handle_t dev_handle) {
    disable_interrupt_1();
    disable_interrupt_2();
    buzzer_resume_winner();
    gpio_set_level(LED_PIN, 0);
    draw_characters(dev_handle, "Player 1 wins",100,10);
    vTaskDelay(2000);
    enable_interrupt_1();
    enable_interrupt_2();
    buzzer_pause();
}
void winner_2_state(spi_device_handle_t dev_handle) {
    disable_interrupt_1();
    disable_interrupt_2();
    buzzer_resume_winner();
    gpio_set_level(LED_PIN, 0);
    draw_characters(dev_handle, "Player 2 wins",100,10);
    vTaskDelay(2000);
    enable_interrupt_1();
    enable_interrupt_2();
    buzzer_pause();
}
void game_end_state(spi_device_handle_t dev_handle) {
    disable_interrupt_1();
    disable_interrupt_2();
    draw_characters(dev_handle, "Game end!",100,10);
    draw_characters(dev_handle,"Final scores!",100,20);
    enable_interrupt_1();
    enable_interrupt_2();
    draw_characters(dev_handle, "Press any button to play new game!", 20, 30);
}