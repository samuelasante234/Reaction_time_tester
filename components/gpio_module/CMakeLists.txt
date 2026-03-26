idf_component_register(
    SRCS "gpio_module.c"
    INCLUDE_DIRS "."
    REQUIRES driver buzzer_module esp_timer timer_module fsm_states_handler
)