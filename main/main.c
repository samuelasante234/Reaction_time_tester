#include <stdio.h>
#include "st7789_header.h"
#include "timer_module.h"
#include "buzzer_module.h"
#include "gpio_module.h"
#include "graphics_module.h"
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
    while (1) {
        switch(fsm_states) {
            case IDLE_STATE:
                st7789_wakeup(st7789_handle);
                fsm_states=WELCOME_STATE;
                break;
            case WELCOME_STATE:
                player1_score=0,player2_score=0;
                draw_characters(st7789_handle,"NEW GAME!",50, 10);
                draw_characters(st7789_handle, "Rules of the game!",50,10);
                fsm_states=TRIGGER_STATE;
                break;
            case TRIGGER_STATE:
                buzzer_pause();
                int dummy=round_count+1;
                char *text = "Round "+dummy;
                draw_characters(st7789_handle, "Game start!",100, 10);
                gpio_set_level(LED_PIN,1);
                fsm_states=NOTHING_STATE;
                break;
            case DISQUALIFIED_1_STATE:
                buzzer_resume_disqualified();
                draw_characters(st7789_handle, "Player 1 disqualified",100,10);
                vTaskDelay(2000);
                gpio_set_level(LED_PIN,0);
                buzzer_pause();
                round_count =0;
                player1_score=0,player2_score=5;
                fsm_states=GAME_END_STATE;
                break;
            case DISQUALIFIED_2_STATE:
                buzzer_resume_disqualified();
                draw_characters(st7789_handle, "Player 2 disqualified",100,10);
                vTaskDelay(2000);
                gpio_set_level(LED_PIN,0);
                buzzer_pause();
                round_count=0;
                player1_score=5,player2_score=0;
                fsm_states=GAME_END_STATE;
                break;
            case WINNER_1_STATE:
                buzzer_resume_winner();
                draw_characters(st7789_handle, "Player 1 wins",100,10);
                vTaskDelay(2000);
                gpio_set_level(LED_PIN, 0);
                buzzer_pause();
                player1_score++, round_count++;
                round_count %=5;
                if (!round_count) fsm_states=GAME_END_STATE;
                else fsm_states=TRIGGER_STATE;
                break;
            case WINNER_2_STATE:
                buzzer_resume_winner();
                draw_characters(st7789_handle, "Player 2 wins",100,10);
                vTaskDelay(2000);
                gpio_set_level(LED_PIN, 0);
                buzzer_pause();
                player2_score++, round_count++;
                round_count %=5;
                if (!round_count) fsm_states=GAME_END_STATE;
                else fsm_states=TRIGGER_STATE;
                break;
            case NOTHING_STATE:
                i%=3;
                break;
            case GAME_END_STATE:
                draw_characters(st7789_handle, "Game end!",100,10);
                draw_characters(st7789_handle,"Final scores!",100,20);
                draw_characters(st7789_handle, "Press any button to play new game!", 20, 30);
                break;
        }
        vTaskDelay(100);
    }
}