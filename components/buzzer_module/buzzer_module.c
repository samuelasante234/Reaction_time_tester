#include "buzzer_module.h"
#include "driver/ledc.h"
#include "stdbool.h"

void buzzer_init();
void buzzer_resume();
void buzzer_pause();

void buzzer_init() {
    ledc_timer_config_t timer_conf ={
        .clk_cfg=LEDC_APB_CLK,
        .duty_resolution=8,
        .freq_hz=4000,
        .timer_num=TIMER_NUM,
        .speed_mode = LEDC_SPEED_MODE
    };
    esp_err_t result=ledc_timer_config(&timer_conf);
    if (result != ESP_OK) {
        printf("Couldn't configure timer! Error: %s\n", esp_err_to_name(result));
        fflush(stdout);
        return;
    }
    ledc_channel_config_t channel_conf ={
        .channel=BUZZER_CHAN,
        .duty=0,
        .gpio_num = BUZZER_NUM,
        .hpoint=0,
        .intr_type=0,
        .timer_sel=TIMER_NUM,
        .sleep_mode = LEDC_SPEED_MODE,
    };
    result = ledc_channel_config(&channel_conf);
    if (result != ESP_OK) {
        printf("Couldn't configure channel! Error: %s\n", esp_err_to_name(result));
        fflush(stdout);
        return;
    }
    
}
void buzzer_pause() {
    esp_err_t result = ledc_set_duty_and_update(LEDC_SPEED_MODE,BUZZER_CHAN,0,0);
    if (result != ESP_OK) {
        printf("Couldn't pause timer! Error: %s\n", esp_err_to_name(result));
        fflush(stdout);
        return;
    }
}
void buzzer_resume(bool *state) {
    esp_err_t result = ledc_set_duty(LEDC_SPEED_MODE, BUZZER_CHAN, 127);
    if (result != ESP_OK) {
        *state = false;
        return;
    };
    result = ledc_update_duty(LEDC_SPEED_MODE, BUZZER_CHAN);
    if (result != ESP_OK) {
        *state = false;
        return;
    };
}