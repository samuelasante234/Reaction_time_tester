#include <stdio.h>
#include "st7789_header.h"
#include "timer_module.h"
#include "buzzer_module.h"
#include "gpio_module.h"
#include "graphics_module.h"
#include "fsm_states_handler.h"
#include <stdint.h>

volatile DRAM_ATTR States fsm_states;
void app_main(void)
{
    buzzer_init();
    gpio_init();
    volatile int i=3;
    gptimer_handle_t timer_handle = timer1_handle();
    spi_device_handle_t st7789_handle= st7789_init();
    vTaskDelay(2000);
    int round_count=0, player1_score=0, player2_score=0;
    interrupts_init(&fsm_states);
    fsm_states=IDLE_STATE;
    while (1) {
        switch(fsm_states) {
            case IDLE_STATE:
                idle_state(st7789_handle, &fsm_states);
                break;
            case WELCOME_STATE:
                player1_score=0,player2_score=0;
                welcome_state(st7789_handle,&fsm_states);
                break;
            case TRIGGER_STATE:
                trigger_state(st7789_handle, &fsm_states, round_count);
                break;
            case DISQUALIFIED_1_STATE:
                disqualified_1_state(st7789_handle, &fsm_states);
                round_count =0;
                player1_score=0,player2_score=5;
                break;
            case DISQUALIFIED_2_STATE:
                disqualified_2_state(st7789_handle, &fsm_states);
                round_count=0;
                player1_score=5,player2_score=0;
                break;
            case WINNER_1_STATE:
                winner_1_state(st7789_handle);
                player1_score++, round_count++;
                round_count %=5;
                if (!round_count) fsm_states=GAME_END_STATE;
                else fsm_states=TRIGGER_STATE;
                break;
            case WINNER_2_STATE:
                winner_2_state(st7789_handle);
                player2_score++, round_count++;
                round_count %=5;
                if (!round_count) fsm_states=GAME_END_STATE;
                else fsm_states=TRIGGER_STATE;
                break;
            case NOTHING_STATE:
                i%=3;
                break;
            case GAME_END_STATE:
                game_end_state(st7789_handle);
                break;
        }
        vTaskDelay(100);
    }
}