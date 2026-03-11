idf_component_register(
    SRCS "fsm_states_handler.c"
    INCLUDE_DIRS "."
    REQUIRES driver st7789 graphics_module timer_module gpio_module buzzer_module font_module
)