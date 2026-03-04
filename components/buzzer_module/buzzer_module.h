#ifndef BUZZER_MODULE_H
#define BUZZER_MODUULE_H

#define BUZZER_NUM 5
#define BUZZER_CHAN 0
#define LEDC_SPEED_MODE LEDC_SPEED_MODE_MAX
#define TIMER_NUM 0
void buzzer_init();
void buzzer_resume();
void buzzer_pause();

#endif