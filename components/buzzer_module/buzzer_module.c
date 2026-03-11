#include "buzzer_module.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"

void buzzer_init();
void buzzer_resume_winner();
void buzzer_resume_disqualified();
void buzzer_pause();

void buzzer_init() {
    ledc_timer_config_t timer_conf ={
        .clk_cfg=LEDC_APB_CLK,
        .duty_resolution=8,
        .freq_hz=8000,
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
        .speed_mode = LEDC_SPEED_MODE,
    };
    result = ledc_channel_config(&channel_conf);
    ledc_fade_func_install(0);
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
void buzzer_resume_winner() {
    esp_err_t result = ledc_set_freq(LEDC_SPEED_MODE, TIMER_NUM,5000);
    if (result != ESP_OK) {
        printf("Couldn't set frequency! Error: %s\n", esp_err_to_name(result));
        fflush(stdout);
        return;
    };
    result = ledc_set_duty(LEDC_SPEED_MODE, BUZZER_CHAN, 127);
    if (result != ESP_OK) {
        printf("Couldn't set duty! Error: %s\n", esp_err_to_name(result));
        fflush(stdout);
        return;
    };
    result = ledc_update_duty(LEDC_SPEED_MODE, BUZZER_CHAN);
    if (result != ESP_OK) {
        printf("Couldn't update duty! Error: %s\n", esp_err_to_name(result));
        fflush(stdout);
        return;
    };
}
void buzzer_resume_disqualified() {
    esp_err_t result = ledc_set_freq(LEDC_SPEED_MODE, TIMER_NUM,2000);
    if (result != ESP_OK) {
        printf("Couldn't set frequency! Error: %s\n", esp_err_to_name(result));
        fflush(stdout);
        return;
    };
    result = ledc_set_duty(LEDC_SPEED_MODE, BUZZER_CHAN, 127);
    if (result != ESP_OK) {
        printf("Couldn't set duty! Error: %s\n", esp_err_to_name(result));
        fflush(stdout);
        return;
    };
    result = ledc_update_duty(LEDC_SPEED_MODE, BUZZER_CHAN);
    if (result != ESP_OK) {
        printf("Couldn't update duty! Error: %s\n", esp_err_to_name(result));
        fflush(stdout);
        return;
    };
}