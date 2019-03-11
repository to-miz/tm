/*
tm_cli.h v0.2.2 - public domain - https://github.com/to-miz/tm
author: Tolga Mizrak 2018

no warranty; use at your own risk

LICENSE
    see license notes at end of file

USAGE
    This file works as both the header and implementation.
    To implement the interfaces in this header,
        #define TM_CLI_IMPLEMENTATION
    in ONE C or C++ source file before #including this header.

PURPOSE
    An alternative to getopt_long that doesn't rely on global state. Thus it can easily be combined with
    loading config files, preprocessing and parsing the same commandline with differing options.

HISTORY
    v0.2.2  10.02.19 Changed const-ness of argv parameters since it was too lenient.
    v0.2.1  03.02.19 Made short options optional if a long name is available.
                     Added error checking for valid options. Options with no shortor long names will
                     produce errors now.
    v0.2    01.02.19 Added TMCLI_USE_WCHAR_T for wchar_t and Windows wmain support.
    v0.1    07.10.18 Initial commit.
*/

/* This is a generated file, do not modify directly. You can find the generator files in the src directory. */

/* clang-format off */

#ifdef TM_CLI_IMPLEMENTATION
    /* Define these to get rid of crt dependency. */

    /* assert */
    #ifndef TM_ASSERT
        #include <assert.h>
        #define TM_ASSERT assert
    #endif /* !defined(TM_ASSERT) */

    #if !defined(TMCLI_USE_WCHAR_T)
        #if !defined(TM_ISDIGIT) || !defined(TM_ISALNUM) || !defined(TM_ISALPHA)
            #include <ctype.h>
        #endif
        #ifndef TM_ISDIGIT
            #define TM_ISDIGIT isdigit
        #endif
        #ifndef TM_ISALNUM
            #define TM_ISALNUM isalnum
        #endif
        #ifndef TM_ISALPHA
            #define TM_ISALPHA isalpha
        #endif
        #if !defined(TM_STRLEN) || !defined(TM_STRNCMP)
            #include <string.h>

            #ifndef TM_STRLEN
                #define TM_STRLEN strlen
            #endif
            #ifndef TM_STRNCMP
                #define TM_STRNCMP strncmp
            #endif
        #endif
    #endif

#endif /* defined(TM_CLI_IMPLEMENTATION) */

#ifndef _TM_CLI_H_INCLUDED_7B17B815_63AF_4D95_B74B_B457740DFB9C_
#define _TM_CLI_H_INCLUDED_7B17B815_63AF_4D95_B74B_B457740DFB9C_

/* Native bools, override by defining TM_BOOL_DEFINED yourself before including this file. */
#ifndef TM_BOOL_DEFINED
    #define TM_BOOL_DEFINED
    #ifdef __cplusplus
        typedef bool tm_bool;
        #define TM_TRUE true
        #define TM_FALSE false
    #else
        typedef _Bool tm_bool;
        #define TM_TRUE 1
        #define TM_FALSE 0
    #endif
#endif /* !defined(TM_BOOL_DEFINED) */

/* size_t is unsigned by default, but we also allow for signed and/or 32bit size_t.
   You can override this block by defining TM_SIZE_T_DEFINED and the typedefs before including this file. */
#ifndef TM_SIZE_T_DEFINED
    #define TM_SIZE_T_DEFINED
    #define TM_SIZE_T_IS_SIGNED 0 /* define to 1 if tm_size_t is signed */
    #include <stddef.h> /* include C version so identifiers are in global namespace */
    typedef size_t tm_size_t;
#endif /* !defined(TM_SIZE_T_DEFINED) */

/* Use null of the underlying language. */
#ifndef TM_NULL
    #ifdef __cplusplus
        #define TM_NULL nullptr
    #else
        #define TM_NULL NULL
    #endif
#endif

/* Fixed width ints. Include C version so identifiers are in global namespace. */
#include <stdint.h>

/* For error logging. */
#include <stdio.h>

/* Linkage defaults to extern, to override define TMCLI_DEF before including this file.
   Examples of possible override values are static or __declspec(dllexport). */
#ifndef TMCLI_DEF
    #define TMCLI_DEF extern
#endif

#ifdef CHAR_BIT
    #define TMCLI_UNSIGNED_BITCOUNT (sizeof(unsigned) * CHAR_BIT)
#else
    #define TMCLI_UNSIGNED_BITCOUNT 32u
#endif

#define TMCLI_GIVEN_OPTIONS_COUNT 4u

#ifdef TMCLI_USE_WCHAR_T
    #include <wchar.h>
    typedef wchar_t tmcli_tchar;
#else
    typedef char tmcli_tchar;
#endif

/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

enum tmcli_argument_type {
    /* Argument types */
    CLI_NO_ARGUMENT,
    CLI_OPTIONAL_ARGUMENT,
    CLI_REQUIRED_ARGUMENT,
};

enum tmcli_option_type_flags {
    CLI_OPTIONAL_OPTION,
    CLI_REQUIRED_OPTION,
};

enum tmcli_output_type {
    CLI_NO_OUTPUT,
    CLI_ASSIGN_OUTPUT,  /* Output will be set like this: *output = output_value; */
    CLI_FLAG_OUTPUT,    /* Output will be set like this: *output |= output_value; */
    CLI_COUNTER_OUTPUT, /* Output will be set like this: *output += output_value; */
};

typedef struct {
    int max_amount; /* Set to a negative number if there is no maximum. */
    unsigned* counter;
} tmcli_multiple;

typedef tm_bool (*tmcli_validator_type)(const tmcli_tchar*);

#define CLI_ARGUMENT_STRING TM_NULL
#define CLI_NO_VALIDATOR TM_NULL
TMCLI_DEF tm_bool CLI_ARGUMENT_INT(const tmcli_tchar* str);
TMCLI_DEF tm_bool CLI_ARGUMENT_UINT(const tmcli_tchar* str);
TMCLI_DEF tm_bool CLI_ARGUMENT_FLOAT(const tmcli_tchar* str);

typedef struct tmcli_option_struct {
    const tmcli_tchar* short_option;
    const tmcli_tchar* long_option;
    int argument_type;
    int option_type;

    tmcli_multiple multiple;
    tmcli_validator_type argument_validator;

    /* For options that have outputs, like options that can be multiple or flags. */
    int output_type;
    unsigned* output;
    unsigned output_value;
} tmcli_option;

typedef struct tmcli_parser_settings_struct {
    FILE* error_log;              /* Error printing will be done into this file. */
    tm_bool allow_free_arguments; /* Whether to allow free arguments that aren't tied to an option, like "input.txt". */
} tmcli_parser_settings;

typedef struct tmcli_parser_struct {
    int argc;
    tmcli_tchar const* const* argv;
    const tmcli_option* options;
    tm_size_t options_count;

    int current;

    uint32_t given_options[TMCLI_GIVEN_OPTIONS_COUNT];
    const tmcli_tchar* current_arg;

    tmcli_parser_settings settings;
    const tmcli_tchar* program_name;
    tm_bool error;
} tmcli_parser;

TMCLI_DEF tmcli_parser_settings tmcli_default_parser_settings();

/*
Creates a cli parser from argc and argv from the main function.
The resulting cli parser uses the default settings returned by tmcli_default_parser_settings.
*/
TMCLI_DEF tmcli_parser tmcli_make_parser(int argc, tmcli_tchar const* const* argv, const tmcli_option* options,
                                         tm_size_t options_count);

/*
Creates a cli parser from given command line arguments and options.
Note that argv[0] must represent the very first command line argument, not the program name.
To call this function from arguments from main directly, you would have to call it like this:
    tmcli_make_parser_ex(argv[0], argc - 1, argv + 1, ...)
See implementation of tmcli_make_parser for how to call this function from argc/argv you get from main.
*/
TMCLI_DEF tmcli_parser tmcli_make_parser_ex(const tmcli_tchar* program_name, int argc, tmcli_tchar const* const* argv,
                                            const tmcli_option* options, tm_size_t options_count,
                                            tmcli_parser_settings settings);

typedef struct tmcli_parsed_option_struct {
    const tmcli_option* option; /* Can be NULL if free arguments are allowed. */
    const tmcli_tchar* argument;
    tm_size_t option_index; /* Only holds a valid value if option != NULL. Refers to parser->options[option_index]. */
} tmcli_parsed_option;

/*
Get the next option from the commandline. Should be called in a while loop to consume the whole commandline.
Params:
    parser:            Parser context to use.
    parsed_option_out: Output parameter of the parsed option. See tmcli_parsed_option for more information.
Returns true if an option was parsed successfully, false otherwise.
*/
tm_bool tmcli_next(tmcli_parser* parser, tmcli_parsed_option* parsed_option_out);

/*
Validates the parser after the whole commandline has been parsed.
Call this to make sure that all required options were provided after parsing is complete.
*/
TMCLI_DEF tm_bool tmcli_validate(tmcli_parser* parser);

#ifdef __cplusplus
}
#endif

#endif /* !defined(_TM_CLI_H_INCLUDED_7B17B815_63AF_4D95_B74B_B457740DFB9C_) */

#ifdef TM_CLI_IMPLEMENTATION
/* clang-format off */
#ifdef TMCLI_USE_WCHAR_T
    #define TMCLI_TEXT(x) L##x
    #define TMCLI_STRFORMAT L"%ls"

    #include <wctype.h>
    #define TMCLI_STRLEN wcslen
    #define TMCLI_STRNCMP wcsncmp
    #define TMCLI_FPRINTF fwprintf

    #define TMCLI_ISDIGIT iswdigit
    #define TMCLI_ISALNUM iswalnum
    #define TMCLI_ISALPHA iswalpha

#else
    #define TMCLI_TEXT(x) x
    #define TMCLI_STRFORMAT "%s"

    #define TMCLI_STRLEN TM_STRLEN
    #define TMCLI_STRNCMP TM_STRNCMP
    #define TMCLI_FPRINTF fprintf

    #define TMCLI_ISDIGIT(x) TM_ISDIGIT((unsigned char)(x))
    #define TMCLI_ISALNUM(x) TM_ISALNUM((unsigned char)(x))
    #define TMCLI_ISALPHA(x) TM_ISALPHA((unsigned char)(x))

#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifndef TM_ASSERT_VALID_SIZE
    #if defined(TM_SIZE_T_IS_SIGNED) && TM_SIZE_T_IS_SIGNED
        #define TM_ASSERT_VALID_SIZE(x) TM_ASSERT((x) >= 0)
    #else
        /* always true if size_t is unsigned */
        #define TM_ASSERT_VALID_SIZE(x) ((void)0)
    #endif
#endif /* !defined(TM_ASSERT_VALID_SIZE) */

#ifdef CHAR_BIT
    /* Include limits.h before including this file to be absolutely correct. */
    #define TMCLI_UNSIGNED_BITCOUNT (sizeof(unsigned) * CHAR_BIT)
#else
    #define TMCLI_UNSIGNED_BITCOUNT 32u
#endif

#define TMCLI_GIVEN_OPTIONS_COUNT 4u

/* clang-format on */

static const tmcli_tchar* tmcli_skip_digits(const tmcli_tchar* str) {
    TM_ASSERT(str);
    if (!TMCLI_ISDIGIT(*str)) return TM_NULL;
    ++str;
    while (*str) {
        if (!TMCLI_ISDIGIT(*str)) return str;
        ++str;
    }
    return str;
}
TMCLI_DEF tm_bool CLI_ARGUMENT_INT(const tmcli_tchar* str) {
    if (!*str) return TM_FALSE;
    if (*str == TMCLI_TEXT('-')) {
        ++str;
        if (!*str) return TM_FALSE;
    }
    str = tmcli_skip_digits(str);
    return str && *str == 0;
}
TMCLI_DEF tm_bool CLI_ARGUMENT_UINT(const tmcli_tchar* str) {
    if (!*str) return TM_FALSE;
    str = tmcli_skip_digits(str);
    return str && *str == 0;
}
TMCLI_DEF tm_bool CLI_ARGUMENT_FLOAT(const tmcli_tchar* str) {
    if (!*str) return TM_FALSE;
    if (*str == TMCLI_TEXT('-')) {
        ++str;
        if (!*str) return TM_FALSE;
    }
    if (*str == TMCLI_TEXT('0')) {
        ++str;
    } else if (*str >= TMCLI_TEXT('1') && *str <= TMCLI_TEXT('9')) {
        ++str;
        const tmcli_tchar* skip = tmcli_skip_digits(str);
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
    if (*str == TMCLI_TEXT('e') || *str == TMCLI_TEXT('E')) {
        ++str;
        if (*str == TMCLI_TEXT('-') || *str == TMCLI_TEXT('+')) ++str;
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

TMCLI_DEF tmcli_parser tmcli_make_parser_ex(const tmcli_tchar* program_name, int argc, tmcli_tchar const* const* argv,
                                            const tmcli_option* options, tm_size_t options_count,
                                            tmcli_parser_settings settings) {
    TM_ASSERT(options_count <= 128);
    tmcli_parser result = {0};
    result.argc = argc;
    result.argv = argv;
    result.options = options;
    result.options_count = options_count;

#if defined(_DEBUG) || !defined(NDEBUG)
    for (tm_size_t i = 0; i < options_count; i++) {
        TM_ASSERT((options[i].short_option && *options[i].short_option) ||
                  (options[i].long_option && *options[i].long_option));
        if ((!options[i].short_option || !*options[i].short_option) &&
            (!options[i].long_option || !*options[i].long_option)) {
            if (settings.error_log) {
                TMCLI_FPRINTF(settings.error_log,
                              TMCLI_TEXT("Option with index %d has neither a short nor a long name.\n"), (int)i);
            }
            result.error = TM_TRUE;
        }
    }
#endif

    result.current = 0;

    result.settings = settings;
    result.program_name = program_name;
    return result;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

TMCLI_DEF tmcli_parser tmcli_make_parser(int argc, tmcli_tchar const* const* argv, const tmcli_option* options,
                                         tm_size_t options_count) {
    TM_ASSERT(argc > 0);
    /* Skip first argument in argv, since it's just the program name. */
    const tmcli_tchar* program_name = argv[0];
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
                    if (option->short_option) {
                        TMCLI_FPRINTF(parser->settings.error_log,
                                      TMCLI_TEXT("Option -") TMCLI_STRFORMAT TMCLI_TEXT(" or --")
                                          TMCLI_STRFORMAT TMCLI_TEXT(" is required.\n"),
                                      option->short_option, option->long_option);
                    } else {
                        TMCLI_FPRINTF(parser->settings.error_log,
                                      TMCLI_TEXT("Option --") TMCLI_STRFORMAT TMCLI_TEXT(" is required.\n"),
                                      option->long_option);
                    }
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

    const tmcli_tchar* current = parser->current_arg;
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
            /* FIXME: Is this correct? Shouldn't it be !*current instead? */
            if (!TMCLI_ISALNUM(*current)) {
                /* Parsing ends since we encountered "--" that isn't followed by an option. */
                return TM_FALSE;
            }

            /* Parse long option. */
            is_short = TM_FALSE;
            tm_bool found = TM_FALSE;
            for (tm_size_t i = 0, count = parser->options_count; i < count; ++i) {
                const tmcli_option* option = &parser->options[i];
                size_t option_length = TMCLI_STRLEN(option->long_option);
                if (TMCLI_STRNCMP(current, option->long_option, option_length) == 0) {
                    const tmcli_tchar last_char = *(current + option_length);
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
                    TMCLI_FPRINTF(parser->settings.error_log,
                                  TMCLI_TEXT("Unrecognized option: \"") TMCLI_STRFORMAT TMCLI_TEXT("\".\n"), current);
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
                    TMCLI_FPRINTF(parser->settings.error_log,
                                  TMCLI_TEXT("Free arguments not allowed: \"") TMCLI_STRFORMAT TMCLI_TEXT("\".\n"),
                                  current);
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
            if (!option->short_option) continue;
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
                TMCLI_FPRINTF(parser->settings.error_log, TMCLI_TEXT("Unrecognized option: \"%c\".\n"), current[0]);
            }
            parser->error = TM_TRUE;
            return TM_FALSE;
        }
        parser->current_arg = current;
    }

    TM_ASSERT(!parser->error);
    const tmcli_option* option = &parser->options[option_index];
    const tmcli_tchar* argument_start = TM_NULL;
    const tmcli_tchar* option_name = (is_short) ? option->short_option : option->long_option;

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
                    TMCLI_FPRINTF(parser->settings.error_log,
                                  TMCLI_TEXT("Option \"") TMCLI_STRFORMAT TMCLI_TEXT("\" was given too many times.\n"),
                                  option_name);
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
        if (!TMCLI_ISALPHA(*current)) {
            if (parser->settings.error_log) {
                TMCLI_FPRINTF(parser->settings.error_log,
                              TMCLI_TEXT("Invalid command line format after option \"")
                                  TMCLI_STRFORMAT TMCLI_TEXT("\".\n"),
                              option_name);
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
                const tmcli_tchar* next = parser->argv[parser->current];
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
                TMCLI_FPRINTF(parser->settings.error_log,
                              TMCLI_TEXT("Option \"") TMCLI_STRFORMAT TMCLI_TEXT("\" takes no arguments.\n"),
                              option_name);
            }
            parser->error = TM_TRUE;
            return TM_FALSE;
        }
        if (current_has_argument && next_is_argument) {
            if (parser->settings.error_log) {
                TMCLI_FPRINTF(parser->settings.error_log,
                              TMCLI_TEXT("Option \"") TMCLI_STRFORMAT TMCLI_TEXT("\" has too many arguments.\n"),
                              option_name);
            }
            parser->error = TM_TRUE;
            return TM_FALSE;
        }
        if (option->argument_type == CLI_REQUIRED_ARGUMENT && (!argument_found || *argument_start == 0)) {
            if (parser->settings.error_log) {
                TMCLI_FPRINTF(parser->settings.error_log,
                              TMCLI_TEXT("Option \"") TMCLI_STRFORMAT TMCLI_TEXT("\" needs an argument.\n"),
                              option_name);
            }
            parser->error = TM_TRUE;
            return TM_FALSE;
        }
        if (argument_start && option->argument_validator && !option->argument_validator(argument_start)) {
            if (parser->settings.error_log) {
                TMCLI_FPRINTF(parser->settings.error_log,
                              TMCLI_TEXT("\"") TMCLI_STRFORMAT TMCLI_TEXT("\" is not a valid argument to option \"")
                                  TMCLI_STRFORMAT TMCLI_TEXT("\".\n"),
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

#endif /* defined(TM_CLI_IMPLEMENTATION) */

/*
There are two licenses you can freely choose from - MIT or Public Domain
---------------------------------------------------------------------------

MIT License:
Copyright (c) 2016 Tolga Mizrak

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---------------------------------------------------------------------------

Public Domain (www.unlicense.org):
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>

---------------------------------------------------------------------------
*/
