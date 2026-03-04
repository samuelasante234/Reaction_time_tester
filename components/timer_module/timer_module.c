#include "timer_module.h"

gptimer_handle_t timer1_handle();

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
    return gptimer_handle;
}
