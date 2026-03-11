#ifndef FSM_STATES_HANDLER_H
#define FSM_STATES_HANDLER_H


void idle_state(spi_device_handle_t dev_handle,States *state);
void welcome_state(spi_device_handle_t dev_handle,States *state);
void trigger_state(spi_device_handle_t dev_handle, States *state, int round);

#endif