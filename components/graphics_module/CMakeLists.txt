idf_component_register(
    SRCS "graphics_module.c"
    INCLUDE_DIRS "."
    REQUIRES driver st7789 font_module esp_mm
)