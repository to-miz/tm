#ifndef TM_LOG_LEVEL
#define TM_LOG_LEVEL
typedef enum {
    tm_log_error = (1u << 0),
    tm_log_warning = (1u << 1),
    tm_log_information = (1u << 2),
    tm_log_debug_information = (1u << 3),
    tm_log_debug_warning = (1u << 4),

    tm_log_all = tm_log_error | tm_log_warning | tm_log_information | tm_log_debug_information |
                 tm_log_debug_warning
} tm_log_level;
#endif