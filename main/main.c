#include <stdio.h>
#include "st7789_header.h"
#include "timer_module.h"
#include "buzzer_module.h"
#include "gpio_module.h"
#include "graphics_module.h"
#include "stdbool.h"
#include <stdint.h>

volatile DRAM_ATTR States fsm_states;
void app_main(void)
{
    buzzer_init();
    gpio_init();
    volatile int i=3;
    gptimer_handle_t timer_handle = timer1_handle();
    spi_device_handle_t st7789_handle= st7789_init();
    interrupts_init(&fsm_states);
    while (1) {
        switch(fsm_states) {
            case IDLE_STATE:
                st7789_wakeup(st7789_handle);
                fsm_states=WELCOME_STATE;
            case WELCOME_STATE:
                draw_characters(st7789_handle,"Welcome to this game!",20, 50, 10);
                fsm_states=TRIGGER_STATE;
            case TRIGGER_STATE:
                draw_characters(st7789_handle, "Game start!",11,100, 10);
                gpio_set_level(LED_PIN,1);
                fsm_states=NOTHING_STATE;
            case DISQUALIFIED_1_STATE:
                draw_characters(st7789_handle, "Player 1 disqualified",21,100,10);
                gpio_set_level(LED_PIN,0);
                fsm_states=NOTHING_STATE;
            case DISQUALIFIED_2_STATE:
                draw_characters(st7789_handle, "Player 2 disqualified",21,100,10);
                gpio_set_level(LED_PIN,0);
                fsm_states=NOTHING_STATE;
            case WINNER_1_STATE:
                draw_characters(st7789_handle, "Player 1 wins",13,100,10);
                gpio_set_level(LED_PIN, 0);
                buzzer_pause();
                fsm_states=NOTHING_STATE;
            case WINNER_2_STATE:
                draw_characters(st7789_handle, "Player 2 wins",13,100,10);
                gpio_set_level(LED_PIN, 0);
                buzzer_pause();
                fsm_states=NOTHING_STATE;
            case NOTHING_STATE:
                i%=3;
        }
    }
}