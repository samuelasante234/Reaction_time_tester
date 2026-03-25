#include "timer_module.h"

gptimer_handle_t timer1_handle();
void timer_start(gptimer_handle_t timer_handle);
void timer_stop(gptimer_handle_t timer_handle);
uint16_t get_time(gptimer_handle_t timer_handle);

gptimer_handle_t timer1_handle() {
    gptimer_config_t timer_conf ={
        .clk_src=GPTIMER_CLK_SRC_APB,
        .direction=GPTIMER_COUNT_UP,
        .intr_priority=0,
        .resolution_hz=1000000,
    };
    gptimer_handle_t gptimer_handle;
    esp_err_t result = gptimer_new_timer(&timer_conf, &gptimer_handle);
    if (result != ESP_OK) {
        printf("Couldn't create timer 1! Error: %s\n", esp_err_to_name(result));
        fflush(stdout);
        return NULL;
    }
    result = gptimer_enable(gptimer_handle);
        if (result != ESP_OK) {
            printf("Couldn't enable timer! Error: %s\n",esp_err_to_name(result));
            fflush(stdout);
            return NULL;
        }
    return gptimer_handle;
}
void timer_start(gptimer_handle_t timer_handle) {
    esp_err_t result = gptimer_set_raw_count(timer_handle,0);
    if (result != ESP_OK) {
        printf("Couldn't start timer! Error: %s\n", esp_err_to_name(result));
        fflush(stdout);
        return;
    }
    result=gptimer_start(timer_handle);
    if (result != ESP_OK) {
        printf("Couldn't start timer! Error: %s\n",esp_err_to_name(result));
        fflush(stdout);
        return;
    }
}
void timer_stop(gptimer_handle_t timer_handle) {
    esp_err_t result = gptimer_stop(timer_handle);
    if (result != ESP_OK) {
        printf("Couldn't stop timer! Error: %s\n",esp_err_to_name(result));
        fflush(stdout);
        return;
    }
}
uint16_t get_time(gptimer_handle_t timer_handle) {
    uint64_t count;
    esp_err_t result = gptimer_get_raw_count(timer_handle, &count);
    if (result != ESP_OK) {
        printf("Couldn't get count! Error: %s\n",esp_err_to_name(result));
        fflush(stdout);
        return 0;
    }
    uint16_t converted = (uint16_t) count;
    return converted;
}
