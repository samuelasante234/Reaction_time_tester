#ifndef BUZZER_MODULE_H
#define BUZZER_MODUULE_H

#define BUZZER_NUM 5
#define BUZZER_CHAN 0
#define LEDC_SPEED_MODE LEDC_LOW_SPEED_MODE
#define TIMER_NUM 0

void buzzer_init();
void buzzer_resume_winner();
void buzzer_resume_disqualified();
void buzzer_pause();

#endif