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
void game_end_state(spi_device_handle_t dev_handle, int play_1_score, int play_2_score);
gptimer_handle_t DRAM_ATTR timer_handle = NULL;

void idle_state(spi_device_handle_t dev_handle, States *state) {
    disable_interrupt_1();
    disable_interrupt_2();
    st7789_wakeup(dev_handle);
    clear_screen(dev_handle);
    vTaskDelay(2000);
    timer_handle=timer1_handle();
    draw_characters(dev_handle,"GROUP 7 PRESENTS", 50,112);
    vTaskDelay(3000);
    draw_characters(dev_handle,"REACTION TIME TESTER",50,112);
    vTaskDelay(4000);
    draw_characters(dev_handle,"GROUP MEMBERS", 50,112);
    vTaskDelay(3000);
    draw_characters(dev_handle,"SETH OSEI ASIEDU",20,112);
    vTaskDelay(3000);
    draw_characters(dev_handle,"SAMUEL OSEI ASANTE",20,112);
    vTaskDelay(3000);
    draw_characters(dev_handle,"BOSOMTWI SERWAA",20,112);
    vTaskDelay(3000);
    draw_characters(dev_handle,"RODDY FRIMPONG",20,112);
    vTaskDelay(3000);
    draw_characters(dev_handle,"SOLOMON",50,112);
    vTaskDelay(3000);
    draw_characters(dev_handle,"JESSICA",50,112);
    vTaskDelay(3000);
    draw_characters(dev_handle,"SAMUELLA BANNERMAN",20,112);
    vTaskDelay(3000);
    draw_characters(dev_handle,"DANIEL GYEBI",20,112);
    vTaskDelay(3000);
    clear_screen(dev_handle);
    vTaskDelay(3000);
    *state=WELCOME_STATE;
}
void welcome_state(spi_device_handle_t dev_handle,States *state) {
    draw_characters(dev_handle,"NEW GAME!",50, 112);
    vTaskDelay(2000);
    clear_screen(dev_handle);
    vTaskDelay(3000);
    //draw_characters(dev_handle,"");
    draw_characters(dev_handle, "RULES OF THE GAME!",50,10);
    vTaskDelay(4000);
    draw_characters(dev_handle, "This is a 2-player game", 0,27);
    vTaskDelay(4000);
    draw_characters(dev_handle,"This game tests your reflexes",0,44);
    vTaskDelay(4000);
    draw_characters(dev_handle,"Disqualification trigger:",0,61);
    draw_characters(dev_handle,"Pressing with LED still off",0,77);
    vTaskDelay(4000);
    draw_characters(dev_handle,"The game is in 5 rounds",0,94);
    vTaskDelay(4000);
    draw_characters(dev_handle,"Disqualification = forfeit",0,111);
    vTaskDelay(4000);
    draw_characters(dev_handle,"Round 5 determines winner;",0,128);
    draw_characters(dev_handle,"if forfeit not occured",0,145);
    vTaskDelay(4000);
    draw_characters(dev_handle, "Enjoy the game! ",20, 162);
    vTaskDelay(4000);
    clear_screen(dev_handle);
    vTaskDelay(3000);
    draw_characters(dev_handle,"Press any button to start",0,112);
    enable_interrupt_1();
    enable_interrupt_2();
    *state=NOTHING_STATE;
}
void trigger_state(spi_device_handle_t dev_handle, States *state, int round) {
    if (!(round)) clear_screen(dev_handle);
    vTaskDelay(3000);
    buzzer_pause();
    int dummy=round+1;
    char text[20];
    snprintf(text,sizeof(text), "Round %d", dummy);
    draw_characters(dev_handle,text,100,112);
    vTaskDelay(2000);
    draw_characters(dev_handle, "Game start!",100, 130);
    vTaskDelay(500);
    gpio_set_level(LED_PIN,1);
    timer_start(timer_handle);
    *state=NOTHING_STATE_2;
}
void disqualified_1_state(spi_device_handle_t dev_handle, States *state) {
    disable_interrupt_1();
    disable_interrupt_2();
    buzzer_resume_disqualified();
    gpio_set_level(LED_PIN,0);
    draw_characters(dev_handle, "Player 1 disqualified",100,130);
    vTaskDelay(2000);
    buzzer_pause();
    *state=GAME_END_STATE;
}
void disqualified_2_state(spi_device_handle_t dev_handle, States *state) {
    disable_interrupt_1();
    disable_interrupt_2();
    buzzer_resume_disqualified();
    gpio_set_level(LED_PIN,0);
    draw_characters(dev_handle, "Player 2 disqualified",100,130);
    vTaskDelay(2000);
    buzzer_pause();
    *state=GAME_END_STATE;
}
void winner_1_state(spi_device_handle_t dev_handle) {
    disable_interrupt_1();
    disable_interrupt_2();
    buzzer_resume_winner();
    uint16_t time_in_ms = get_time(timer_handle);
    char str_time_in_ms[30];
    snprintf(str_time_in_ms,sizeof(str_time_in_ms),"Reaction time: %dms", time_in_ms);
    gpio_set_level(LED_PIN, 0);
    draw_characters(dev_handle, "Player 1 wins",80,112);
    vTaskDelay(2000);
    draw_characters(dev_handle,str_time_in_ms,60, 130);
    vTaskDelay(2000);
    enable_interrupt_1();
    enable_interrupt_2();
    buzzer_pause();
}
void winner_2_state(spi_device_handle_t dev_handle) {
    disable_interrupt_1();
    disable_interrupt_2();
    buzzer_resume_winner();
    uint16_t time_in_ms = get_time(timer_handle);
    char str_time_in_ms[30];
    snprintf(str_time_in_ms,sizeof(str_time_in_ms),"Reaction time: %dms", time_in_ms);
    gpio_set_level(LED_PIN, 0);
    draw_characters(dev_handle, "Player 2 wins",80,112);
    vTaskDelay(2000);
    draw_characters(dev_handle,str_time_in_ms,60, 130);
    vTaskDelay(2000);
    enable_interrupt_1();
    enable_interrupt_2();
    buzzer_pause();
}
void game_end_state(spi_device_handle_t dev_handle, int play_1_score, int play_2_score) {
    disable_interrupt_1();
    disable_interrupt_2();
    draw_characters(dev_handle, "GAME END!",80,112);
    vTaskDelay(2000);
    draw_characters(dev_handle,"Final scores!",100,112);
    vTaskDelay(2000);
    char final_score_message[25];
    snprintf(final_score_message,sizeof(final_score_message),"Player 1 score: %d",play_1_score);
    draw_characters(dev_handle,final_score_message,0,130);
    vTaskDelay(1000);
    snprintf(final_score_message,sizeof(final_score_message),"Player 2 score: %d",play_2_score);
    draw_characters(dev_handle,final_score_message,0,150);
    vTaskDelay(2000);
    if (play_1_score > play_2_score) {
        draw_characters(dev_handle,"Winner: Player 1",20,112);
        vTaskDelay(2000);
    }
    else {
        draw_characters(dev_handle,"Winner: Player 2",20,112);
        vTaskDelay(2000);
    }
    clear_screen(dev_handle);
    vTaskDelay(2000);
    enable_interrupt_1();
    enable_interrupt_2();
    draw_characters(dev_handle, "Press any button to play new game!", 20, 30);
}