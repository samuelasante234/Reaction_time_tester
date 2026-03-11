#ifndef GPIO_MODULE_H
#define GPIO_MODULE_H

typedef enum {
    IDLE_STATE,
    WELCOME_STATE,
    TRIGGER_STATE,
    DISQUALIFIED_1_STATE,
    DISQUALIFIED_2_STATE,
    WINNER_1_STATE,
    WINNER_2_STATE,
    NOTHING_STATE,
    GAME_END_STATE,
} States;
#define LED_PIN 1
#define BUTTON_1_PIN 2
#define BUTTON_2_PIN 7

void gpio_init();
void interrupts_init(States *fsm_state);
void disable_interrupt_1();
void disable_interrupt_2();
void enable_interrupt_1();
void enable_interrupt_2();

#endif