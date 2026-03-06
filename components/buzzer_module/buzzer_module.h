#ifndef BUZZER_MODULE_H
#define BUZZER_MODUULE_H

#define BUZZER_NUM 5
#define BUZZER_CHAN 0
#define LEDC_SPEED_MODE LEDC_SPEED_MODE_MAX
#define TIMER_NUM 0

#include <stdbool.h>

void buzzer_init();
void buzzer_resume(bool *state);
void buzzer_pause();

#endif