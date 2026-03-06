idf_component_register(
    SRCS "gpio_module.c"
    INCLUDE_DIRS "."
    REQUIRES driver buzzer_module
)