#ifndef TIMER_MODULE_H
#define TIMER_MODULE_H

#define TIMER_FREQUENCY 1000000

#include "driver/gptimer.h"
#include "driver/ledc.h"

extern gptimer_handle_t timer1_handle();
void timer_start(gptimer_handle_t timer_handle);
void timer_stop(gptimer_handle_t timer_handle);
uint16_t get_time(gptimer_handle_t timer_handle);

#endif