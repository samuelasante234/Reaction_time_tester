#include "gpio_module.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "buzzer_module.h"
#include <stdbool.h>

void gpio_init();
void interrupts_init(States *fsm_state);
static void IRAM_ATTR IRS_BUTTON_1(void *arg);
static void IRAM_ATTR IRS_BUTTON_2(void *arg);
static DRAM_ATTR bool err_debug_variable;

void gpio_init() {
    gpio_config_t ledpin_conf = {
        .mode=GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask=1ULL<<LED_PIN,
        .pull_down_en=1,
    };
    gpio_config(&ledpin_conf);
    gpio_config_t button1_pin_conf = {
        .mode=GPIO_MODE_INPUT,
        .pin_bit_mask=1ULL<<BUTTON_1_PIN,
        .pull_down_en=1,
        .intr_type=GPIO_INTR_POSEDGE,
    };
    gpio_config(&button1_pin_conf);
    gpio_config_t button2_pin_conf = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = 1ULL<<BUTTON_2_PIN,
        .pull_down_en=1,
        .intr_type=GPIO_INTR_POSEDGE,
    };
    gpio_config(&button2_pin_conf);
}
void interrupts_init(States *fsm_state) {
    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED|ESP_INTR_FLAG_EDGE|ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add(BUTTON_1_PIN, IRS_BUTTON_1, fsm_state);
    gpio_isr_handler_add(BUTTON_2_PIN, IRS_BUTTON_2, fsm_state);
}

static void IRAM_ATTR IRS_BUTTON_1(void *arg) {
    esp_err_t result = gpio_intr_disable(BUTTON_2_PIN);
    if (result != ESP_OK) return;
    States *s = (States*) arg;
    if (gpio_get_level(LED_PIN)) {
        buzzer_resume(&err_debug_variable);
        *s = WINNER_1_STATE;
    }
    else {
        *s=DISQUALIFIED_1_STATE;
    }
    result = gpio_intr_enable(BUTTON_2_PIN);
    if (result != ESP_OK) return;
}
static void IRAM_ATTR IRS_BUTTON_2(void *arg) {
    esp_err_t result = gpio_intr_disable(BUTTON_1_PIN);
    if (result != ESP_OK) return;
    States *s = (States*) arg;
    if (gpio_get_level(LED_PIN)) {
        buzzer_resume(&err_debug_variable);
        *s = WINNER_2_STATE;
    }
    else {
        *s=DISQUALIFIED_2_STATE;
    }
    result = gpio_intr_enable(BUTTON_1_PIN);
    if (result != ESP_OK) return;
}