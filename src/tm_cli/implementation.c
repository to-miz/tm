#ifdef __cplusplus
extern "C" {
#endif

/* clang-format off */
#include <stdint.h>

#include "../common/tm_assert_valid_size.inc"

#ifdef CHAR_BIT
    /* Include limits.h before including this file to be absolutely correct. */
    #define TMCLI_UNSIGNED_BITCOUNT (sizeof(unsigned) * CHAR_BIT)
#else
    #define TMCLI_UNSIGNED_BITCOUNT 32u
#endif

#define TMCLI_GIVEN_OPTIONS_COUNT 4u

/* clang-format on */

static const char* tmcli_skip_digits(const char* str) {
    TM_ASSERT(str);
    if (!TM_ISDIGIT((unsigned char)*str)) {
        return TM_NULL;
    }
    ++str;
    while (*str) {
        if (!TM_ISDIGIT((unsigned char)*str)) {
            return str;
        }
        ++str;
    }
    return str;
}
TMCLI_DEF tm_bool CLI_ARGUMENT_INT(const char* str) {
    if (!*str) return TM_FALSE;
    if (*str == '-') {
        ++str;
        if (!*str) return TM_FALSE;
    }
    str = tmcli_skip_digits(str);
    return str && *str == 0;
}
TMCLI_DEF tm_bool CLI_ARGUMENT_UINT(const char* str) {
    if (!*str) return TM_FALSE;
    str = tmcli_skip_digits(str);
    return str && *str == 0;
}
TMCLI_DEF tm_bool CLI_ARGUMENT_FLOAT(const char* str) {
    if (!*str) return TM_FALSE;
    if (*str == '-') {
        ++str;
        if (!*str) return TM_FALSE;
    }
    if (*str == '0') {
        ++str;
    } else if (*str >= '1' && *str <= '9') {
        ++str;
        const char* skip = tmcli_skip_digits(str);
        if (skip) str = skip;
    } else {
        return TM_FALSE;
    }
    if (!*str) return TM_TRUE;
    if (*str == '.') {
        ++str;
        str = tmcli_skip_digits(str);
        if (!str) return TM_FALSE;
        if (!*str) return TM_TRUE;
    }
    if (*str == 'e' || *str == 'E') {
        ++str;
        if (*str == '-' || *str == '+') {
            ++str;
        }
        str = tmcli_skip_digits(str);
        if (!str) return TM_FALSE;
    }
    return !*str;
}

/* Ignore missing-field-initializers warning on gcc/clang, because we use {0} to zero initialize. */
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

TMCLI_DEF tmcli_parser_settings tmcli_default_parser_settings() {
    tmcli_parser_settings result = {0};
    result.error_log = stderr;
    result.allow_free_arguments = TM_TRUE;
    return result;
}

TMCLI_DEF tmcli_parser tmcli_make_parser_ex(const char* program_name, int argc, char const** argv,
                                            const tmcli_option* options, tm_size_t options_count,
                                            tmcli_parser_settings settings) {
    TM_ASSERT(options_count <= 128);
    tmcli_parser result = {0};
    result.argc = argc;
    result.argv = argv;
    result.options = options;
    result.options_count = options_count;

    result.current = 0;

    result.settings = settings;
    result.program_name = program_name;
    return result;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

TMCLI_DEF tmcli_parser tmcli_make_parser(int argc, char const** argv, const tmcli_option* options,
                                         tm_size_t options_count) {
    TM_ASSERT(argc > 0);
    /* Skip first argument in argv, since it's just the program name. */
    const char* program_name = argv[0];
    return tmcli_make_parser_ex(program_name, argc - 1, argv + 1, options, options_count,
                                tmcli_default_parser_settings());
}

typedef struct {
    unsigned index;
    unsigned flag;
} tmcli_flag_pair;

static tmcli_flag_pair tmcli_get_flag_pair(tm_size_t index) {
    TM_ASSERT_VALID_SIZE(index);

    tmcli_flag_pair result;
    result.index = (unsigned)(index / TMCLI_UNSIGNED_BITCOUNT);
    unsigned shift_amount = (unsigned)(index % TMCLI_UNSIGNED_BITCOUNT);
    TM_ASSERT(shift_amount < TMCLI_UNSIGNED_BITCOUNT);
    result.flag = 1u << shift_amount;

    TM_ASSERT(result.index < TMCLI_GIVEN_OPTIONS_COUNT);
    TM_ASSERT(result.flag != 0);
    return result;
}

TMCLI_DEF tm_bool tmcli_validate(tmcli_parser* parser) {
    if (parser->error) {
        return TM_FALSE;
    }

    /* Check that all required options were given. */
    for (tm_size_t i = 0, count = parser->options_count; i < count; ++i) {
        const tmcli_option* option = &parser->options[i];
        if (option->option_type == CLI_REQUIRED_OPTION) {
            tmcli_flag_pair pair = tmcli_get_flag_pair(i);
            if (!(parser->given_options[pair.index] & pair.flag)) {
                if (parser->settings.error_log) {
                    fprintf(parser->settings.error_log, "Option -%s or --%s is required.\n", option->short_option,
                            option->long_option);
                }
                parser->error = TM_TRUE;
                return TM_FALSE;
            }
        }
    }
    return TM_TRUE;
}

TMCLI_DEF tm_bool tmcli_next(tmcli_parser* parser, tmcli_parsed_option* parsed_option_out) {
    TM_ASSERT(!parser->error);

    const char* current = parser->current_arg;
    if (!current || !*current) {
        if (parser->current >= parser->argc) {
            return TM_FALSE;
        }
        current = parser->argv[parser->current];
        ++parser->current;
    }

    tm_size_t option_index = 0;
    tm_bool is_short = TM_TRUE;

    if (*current == '-') {
        is_short = TM_TRUE;
        ++current;
        if (*current == '-') {
            ++current;
            if (!TM_ISALNUM((unsigned char)(*current))) {
                /* Parsing ends since we encountered "--" that isn't followed by an option. */
                return TM_FALSE;
            }

            /* Parse long option. */
            is_short = TM_FALSE;
            tm_bool found = TM_FALSE;
            for (tm_size_t i = 0, count = parser->options_count; i < count; ++i) {
                const tmcli_option* option = &parser->options[i];
                size_t option_length = strlen(option->long_option);
                if (strncmp(current, option->long_option, option_length) == 0) {
                    const char last_char = *(current + option_length);
                    if (last_char == 0 || last_char == '=') {
                        /* Matched completely to an option */
                        current += option_length;
                        option_index = i;
                        found = TM_TRUE;
                        break;
                    }
                }
            }
            if (!found) {
                if (parser->settings.error_log) {
                    fprintf(parser->settings.error_log, "Unrecognized option: \"%s\".\n", current);
                }
                parser->error = TM_TRUE;
                return TM_FALSE;
            }
            parser->current_arg = TM_NULL;
        }
    } else {
        if (parser->current_arg) {
            /* Current arg is part of multiple short options, like -abc. */
        } else {
            /* Current arg is an argument without an option, like input.txt */
            if (!parser->settings.allow_free_arguments) {
                if (parser->settings.error_log) {
                    fprintf(parser->settings.error_log, "Free arguments not allowed: \"%s\".\n", current);
                }
                parser->error = TM_TRUE;
                return TM_FALSE;
            }
            if (parsed_option_out) {
                parsed_option_out->option_index = 0;
                parsed_option_out->option = TM_NULL;
                parsed_option_out->argument = current;
            }
            return TM_TRUE;
        }
        is_short = TM_TRUE;
    }

    if (is_short) {
        /* Parse short option. */
        tm_bool found = TM_FALSE;
        for (tm_size_t i = 0, count = parser->options_count; i < count; ++i) {
            const tmcli_option* option = &parser->options[i];
            TM_ASSERT(option->short_option[0] != 0);
            if (current[0] == option->short_option[0]) {
                ++current;
                option_index = i;
                found = TM_TRUE;
                break;
            }
        }
        if (!found) {
            if (parser->settings.error_log) {
                fprintf(parser->settings.error_log, "Unrecognized option: \"%c\".\n", current[0]);
            }
            parser->error = TM_TRUE;
            return TM_FALSE;
        }
        parser->current_arg = current;
    }

    TM_ASSERT(!parser->error);
    const tmcli_option* option = &parser->options[option_index];
    const char* argument_start = TM_NULL;
    const char* option_name = (is_short) ? option->short_option : option->long_option;

    {
        unsigned counter = 0;
        if (option->multiple.counter) {
            ++*option->multiple.counter;
            counter = *option->multiple.counter;
        }

        /* Check if we encountered this option already. */
        tmcli_flag_pair pair = tmcli_get_flag_pair(option_index);
        if (parser->given_options[pair.index] & pair.flag) {
            /* We encountered option before. */
            if (option->multiple.max_amount == 0 || option->multiple.max_amount == 1 ||
                (option->multiple.max_amount > 0 && counter > (unsigned)option->multiple.max_amount)) {
                if (parser->settings.error_log) {
                    fprintf(parser->settings.error_log, "Option \"%s\" was given too many times.\n", option_name);
                }
                parser->error = TM_TRUE;
                return TM_FALSE;
            }
        }
        parser->given_options[pair.index] |= pair.flag;
    }

    /* Output of option value. */
    if (option->output) {
        switch (option->output_type) {
            case CLI_ASSIGN_OUTPUT: {
                *option->output = option->output_value;
                break;
            }
            case CLI_FLAG_OUTPUT: {
                *option->output |= option->output_value;
                break;
            }
            case CLI_COUNTER_OUTPUT: {
                *option->output += option->output_value;
                break;
            }
        }
    }

    if (is_short && *current != 0 && option->argument_type == CLI_NO_ARGUMENT) {
        /* Commandline argument is in the form of multiple short options, like -abc */
        if (!isalpha(*current)) {
            if (parser->settings.error_log) {
                fprintf(parser->settings.error_log, "Invalid command line format after option \"%s\".\n", option_name);
            }
            parser->error = TM_TRUE;
            return TM_FALSE;
        }
    } else {
        /* Parse argument. */
        tm_bool argument_found = TM_FALSE;
        tm_bool next_is_argument = TM_FALSE;

        if (*current != '=') {
            if (parser->current < parser->argc) {
                const char* next = parser->argv[parser->current];
                if (*next != '-') {
                    next_is_argument = TM_TRUE;
                    argument_found = TM_TRUE;
                    argument_start = next;
                    ++parser->current;
                }
            }
        }

        tm_bool current_has_argument = TM_FALSE;
        if (*current != 0) {
            if (*current == '=') {
                ++current;
            }

            current_has_argument = TM_TRUE;
            argument_start = current;
            argument_found = TM_TRUE;
        }

        TM_ASSERT(argument_start || !argument_found);

        if (option->argument_type == CLI_NO_ARGUMENT && argument_found) {
            if (parser->settings.error_log) {
                fprintf(parser->settings.error_log, "Option \"%s\" takes no arguments.\n", option_name);
            }
            parser->error = TM_TRUE;
            return TM_FALSE;
        }
        if (current_has_argument && next_is_argument) {
            if (parser->settings.error_log) {
                fprintf(parser->settings.error_log, "Option \"%s\" has too many arguments.\n", option_name);
            }
            parser->error = TM_TRUE;
            return TM_FALSE;
        }
        if (option->argument_type == CLI_REQUIRED_ARGUMENT && (!argument_found || *argument_start == 0)) {
            if (parser->settings.error_log) {
                fprintf(parser->settings.error_log, "Option \"%s\" needs an argument.\n", option_name);
            }
            parser->error = TM_TRUE;
            return TM_FALSE;
        }
        if (argument_start && option->argument_validator && !option->argument_validator(argument_start)) {
            if (parser->settings.error_log) {
                fprintf(parser->settings.error_log, "\"%s\" is not a valid argument to option \"%s\".\n",
                        argument_start, option_name);
            }
            parser->error = TM_TRUE;
            return TM_FALSE;
        }
    }

    if (argument_start) {
        parser->current_arg = TM_NULL;
    }

    if (parsed_option_out) {
        parsed_option_out->option_index = option_index;
        parsed_option_out->option = option;
        parsed_option_out->argument = argument_start;
    }
    return TM_TRUE;
}

#ifdef __cplusplus
}
#endif
