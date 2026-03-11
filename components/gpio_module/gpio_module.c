#include "gpio_module.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "buzzer_module.h"
#include "esp_timer.h"

void gpio_init();
void interrupts_init(States *fsm_state);
void disable_interrupt_1();
void disable_interrupt_2();
void enable_interrupt_1();
void enable_interrupt_2();
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
    gpio_set_drive_capability(LED_PIN, 2);
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
    esp_err_t result = esp_timer_init();
    if (result != ESP_OK) {
        printf("Couldn't initialise timer! Error: %s\n", esp_err_to_name(result));
        fflush(stdout);
        return;
    }
}
void interrupts_init(States *fsm_state) {
    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED|ESP_INTR_FLAG_EDGE|ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add(BUTTON_1_PIN, IRS_BUTTON_1, fsm_state);
    gpio_isr_handler_add(BUTTON_2_PIN, IRS_BUTTON_2, fsm_state);
}
void disable_interrupt_1() {
    esp_err_t result = gpio_intr_disable(BUTTON_1_PIN);
    if (result != ESP_OK) {
        printf("Couldn't disable interrupt on button 1! Error: %s\n",esp_err_to_name(result));
        fflush(stdout);
        return;
    }
}
void disable_interrupt_2() {
    esp_err_t result = gpio_intr_disable(BUTTON_2_PIN);
    if (result != ESP_OK) {
        printf("Couldn't disable interrupt on button 2! Error: %s\n",esp_err_to_name(result));
        fflush(stdout);
        return;
    }
}
void enable_interrupt_1() {
    esp_err_t result = gpio_intr_enable(BUTTON_1_PIN);
    if (result != ESP_OK) {
        printf("Couldn't enable interrupt on button 1! Error: %s\n",esp_err_to_name(result));
        fflush(stdout);
        return;
    }
}
void enable_interrupt_2() {
    esp_err_t result = gpio_intr_enable(BUTTON_2_PIN);
    if (result != ESP_OK) {
        printf("Couldn't enable interrupt on button 2! Error: %s\n",esp_err_to_name(result));
        fflush(stdout);
        return;
    }
}

static void IRAM_ATTR IRS_BUTTON_1(void *arg) {
    static int64_t initial=0;
    int64_t final = esp_timer_get_time();
    if ((final-initial)<20000) return;
    esp_err_t result = gpio_intr_disable(BUTTON_2_PIN);
    if (result != ESP_OK) return;
    volatile States *s = (volatile States*) arg;
    if (*s == GAME_END_STATE) {
        *s = WELCOME_STATE;
        return;
    }
    if (gpio_get_level(LED_PIN)) {
        *s = WINNER_1_STATE;
    }
    else {
        *s=DISQUALIFIED_1_STATE;
    }
    initial=final;
    result = gpio_intr_enable(BUTTON_2_PIN);
    if (result != ESP_OK) return;
}
static void IRAM_ATTR IRS_BUTTON_2(void *arg) {
    static int64_t initial=0;
    int64_t final=esp_timer_get_time();
    if ((final-initial)<20000) return;
    esp_err_t result = gpio_intr_disable(BUTTON_1_PIN);
    if (result != ESP_OK) return;    
    volatile States *s = (volatile States*) arg;
    if (*s == GAME_END_STATE) {
        *s = WELCOME_STATE;
        return;
    }
    if (gpio_get_level(LED_PIN)) {
        *s = WINNER_2_STATE;
    }
    else {
        *s=DISQUALIFIED_2_STATE;
    }
    initial=final;
    result = gpio_intr_enable(BUTTON_1_PIN);
    if (result != ESP_OK) return;
}