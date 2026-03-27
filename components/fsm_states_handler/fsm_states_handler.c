#include <stdio.h>
#include "st7789_header.h"
#include "timer_module.h"
#include "buzzer_module.h"
#include "gpio_module.h"
#include "graphics_module.h"
#include <stdint.h>
#include <string.h>
#include "fsm_states_handler.h"

#define DELAY_1_S 1000
#define DELAY_2_S 2000
#define DELAY_3_S 3000
#define DELAY_4_S 4000
#define DELAY_5_S 5000

void idle_state(spi_device_handle_t dev_handle,States *state);
void welcome_state(spi_device_handle_t dev_handle,States *state);
void trigger_state(spi_device_handle_t dev_handle, States *state, int round);
void disqualified_1_state(spi_device_handle_t dev_handle, States *state);
void disqualified_2_state(spi_device_handle_t dev_handle, States *state);
void winner_1_state(spi_device_handle_t dev_handle);
void winner_2_state(spi_device_handle_t dev_handle);
void game_end_state(spi_device_handle_t dev_handle, int play_1_score, int play_2_score);
gptimer_handle_t DRAM_ATTR timer_handle = NULL;

void idle_state(spi_device_handle_t dev_handle, States *state) {
    disable_interrupt_1();
    disable_interrupt_2();
    st7789_wakeup(dev_handle);
    clear_screen(dev_handle);
    vTaskDelay(DELAY_2_S);
    timer_handle=timer1_handle();
    draw_characters(dev_handle,"SAMUEL PRESENTS...",20,112);
    vTaskDelay(DELAY_3_S);
    /*
    draw_characters(dev_handle,"GROUP 7 PRESENTS", 50,112);
    vTaskDelay(DELAY_3_S);*/
    draw_characters(dev_handle,"REACTION TIME TESTER",50,112);
    vTaskDelay(DELAY_4_S);
    /*
    draw_characters(dev_handle,"GROUP MEMBERS", 50,112);
    vTaskDelay(DELAY_3_S);
    draw_characters(dev_handle,"SETH OSEI ASIEDU",20,112);
    vTaskDelay(DELAY_3_S);
    draw_characters(dev_handle,"SAMUEL OSEI ASANTE",20,112);
    vTaskDelay(DELAY_3_S);
    draw_characters(dev_handle,"BOSOMTWI SERWAA",20,112);
    vTaskDelay(DELAY_3_S);
    draw_characters(dev_handle,"RODDY FRIMPONG",20,112);
    vTaskDelay(DELAY_3_S);
    draw_characters(dev_handle,"SOLOMON",50,112);
    vTaskDelay(DELAY_3_S);
    draw_characters(dev_handle,"JESSICA",50,112);
    vTaskDelay(DELAY_3_S);
    draw_characters(dev_handle,"SAMUELLA BANNERMAN",20,112);
    vTaskDelay(DELAY_3_S);
    draw_characters(dev_handle,"DANIEL GYEBI",20,112);
    vTaskDelay(DELAY_3_S);
    draw_characters(dev_handle,"KAMIL KHADIJA",20,112);
    vTaskDelay(DELAY_3_S);*/
    
    clear_screen(dev_handle);
    vTaskDelay(DELAY_3_S);
    *state=WELCOME_STATE;
}
void welcome_state(spi_device_handle_t dev_handle,States *state) {
    clear_screen(dev_handle);
    vTaskDelay(DELAY_2_S);
    draw_characters(dev_handle,"NEW GAME!",50, 112);
    vTaskDelay(DELAY_2_S);
    clear_screen(dev_handle);
    vTaskDelay(DELAY_3_S);
    draw_characters(dev_handle, "RULES OF THE GAME!",50,10);
    vTaskDelay(DELAY_4_S);
    draw_characters(dev_handle, "This is a 2-player game", 0,27);
    vTaskDelay(DELAY_4_S);
    draw_characters(dev_handle,"This game tests your reflexes",0,44);
    vTaskDelay(DELAY_4_S);
    draw_characters(dev_handle,"Disqualification trigger:",0,61);
    draw_characters(dev_handle,"Pressing with LED still off",0,77);
    vTaskDelay(DELAY_4_S);
    draw_characters(dev_handle,"The game is in 5 rounds",0,94);
    vTaskDelay(DELAY_4_S);
    draw_characters(dev_handle,"Disqualification = forfeit",0,111);
    vTaskDelay(DELAY_4_S);
    draw_characters(dev_handle,"Round 5 determines winner;",0,128);
    draw_characters(dev_handle,"if forfeit not occured",0,145);
    vTaskDelay(DELAY_4_S);
    clear_screen(dev_handle);
    vTaskDelay(DELAY_3_S);
    draw_characters(dev_handle, "ENJOY THE GAME! ",20, 112);
    vTaskDelay(DELAY_4_S);
    clear_screen(dev_handle);
    vTaskDelay(DELAY_3_S);
    draw_characters(dev_handle,"Press any button to start",10,112);
    enable_interrupt_1();
    enable_interrupt_2();
    *state=NOTHING_STATE;
}
void trigger_state(spi_device_handle_t dev_handle, States *state, int round) {
    if (!(round)) clear_screen(dev_handle);
    vTaskDelay(DELAY_3_S);
    buzzer_pause();
    int dummy=round+1;
    char text[20];
    snprintf(text,sizeof(text), "Round %d", dummy);
    draw_characters(dev_handle,text,90,112);
    vTaskDelay(DELAY_2_S);
    draw_characters(dev_handle, "Game start!",75, 130);
    uint16_t delay = random_delay();volatile uint16_t track=0;
    while (track < delay) {
        if (*state != TRIGGER_STATE) return;
        track++;
        vTaskDelay(1);
    }
    gpio_set_level(LED_PIN,1);
    timer_start(timer_handle);
    *state=NOTHING_STATE_2;
}
void disqualified_1_state(spi_device_handle_t dev_handle, States *state) {
    disable_interrupt_1();
    disable_interrupt_2();
    buzzer_resume_disqualified();
    gpio_set_level(LED_PIN,0);
    draw_characters(dev_handle, "Player 1 disqualified",20,130);
    vTaskDelay(DELAY_2_S);
    buzzer_pause();
    *state=GAME_END_STATE;
}
void disqualified_2_state(spi_device_handle_t dev_handle, States *state) {
    disable_interrupt_1();
    disable_interrupt_2();
    buzzer_resume_disqualified();
    gpio_set_level(LED_PIN,0);
    draw_characters(dev_handle, "Player 2 disqualified",20,130);
    vTaskDelay(DELAY_2_S);
    buzzer_pause();
    *state=GAME_END_STATE;
}
void winner_1_state(spi_device_handle_t dev_handle) {
    disable_interrupt_1();
    disable_interrupt_2();
    buzzer_resume_winner();
    uint16_t time_in_us = get_time(timer_handle);
    char str_time_in_us[30];
    snprintf(str_time_in_us,sizeof(str_time_in_us),"Reaction time: %dus", time_in_us);
    gpio_set_level(LED_PIN, 0);
    draw_characters(dev_handle, "Player 1 wins",80,112);
    vTaskDelay(DELAY_2_S);
    draw_characters(dev_handle,str_time_in_us,60, 130);
    vTaskDelay(DELAY_2_S);
    enable_interrupt_1();
    enable_interrupt_2();
    buzzer_pause();
}
void winner_2_state(spi_device_handle_t dev_handle) {
    disable_interrupt_1();
    disable_interrupt_2();
    buzzer_resume_winner();
    uint16_t time_in_us = get_time(timer_handle);
    char str_time_in_us[30];
    snprintf(str_time_in_us,sizeof(str_time_in_us),"Reaction time: %dus", time_in_us);
    gpio_set_level(LED_PIN, 0);
    draw_characters(dev_handle, "Player 2 wins",80,112);
    vTaskDelay(DELAY_2_S);
    draw_characters(dev_handle,str_time_in_us,60, 130);
    vTaskDelay(DELAY_2_S);
    enable_interrupt_1();
    enable_interrupt_2();
    buzzer_pause();
}
void game_end_state(spi_device_handle_t dev_handle, int play_1_score, int play_2_score) {
    disable_interrupt_1();
    disable_interrupt_2();
    draw_characters(dev_handle, "GAME END!",80,112);
    vTaskDelay(DELAY_2_S);
    draw_characters(dev_handle,"Final scores!",100,112);
    vTaskDelay(DELAY_2_S);
    char final_score_message[25];
    snprintf(final_score_message,sizeof(final_score_message),"Player 1 score: %d",play_1_score);
    draw_characters(dev_handle,final_score_message,0,130);
    vTaskDelay(DELAY_1_S);
    snprintf(final_score_message,sizeof(final_score_message),"Player 2 score: %d",play_2_score);
    draw_characters(dev_handle,final_score_message,0,150);
    vTaskDelay(DELAY_2_S);
    if (play_1_score > play_2_score) {
        draw_characters(dev_handle,"Winner: Player 1",20,112);
        vTaskDelay(DELAY_2_S);
    }
    else {
        draw_characters(dev_handle,"Winner: Player 2",20,112);
        vTaskDelay(DELAY_2_S);
    }
    clear_screen(dev_handle);
    vTaskDelay(DELAY_2_S);
    enable_interrupt_1();
    enable_interrupt_2();
    draw_characters(dev_handle, "Press a button for new game", 20, 112);
}