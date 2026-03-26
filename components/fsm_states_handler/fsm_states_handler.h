#ifndef FSM_STATES_HANDLER_H
#define FSM_STATES_HANDLER_H

#include "driver/spi_master.h"
#include "driver/spi_common.h"
void idle_state(spi_device_handle_t dev_handle,States *state);
void welcome_state(spi_device_handle_t dev_handle,States *state);
void trigger_state(spi_device_handle_t dev_handle, States *state, int round);
void disqualified_1_state(spi_device_handle_t dev_handle, States *state);
void disqualified_2_state(spi_device_handle_t dev_handle, States *state);
void winner_1_state(spi_device_handle_t dev_handle);
void winner_2_state(spi_device_handle_t dev_handle);
void game_end_state(spi_device_handle_t dev_handle, int play_1_score, int play_2_score);
extern gptimer_handle_t DRAM_ATTR timer_handle;

#endif