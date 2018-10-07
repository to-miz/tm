/*
tm_cli.h v0.1 - public domain - https://github.com/to-miz/tm
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
    v0.1   07.10.18 initial commit
*/

#include "../common/GENERATED_WARNING.inc"

/* clang-format off */

#ifdef TM_CLI_IMPLEMENTATION
    /* Define these to get rid of crt dependency. */

    #include "../common/tm_assert.inc"

    #if !defined(TM_ISDIGIT) || !defined(TM_ISALNUM)
        #include <ctype.h>
    #endif
    #ifndef TM_ISDIGIT
        #define TM_ISDIGIT isdigit
    #endif
    #ifndef TM_ISALNUM
        #define TM_ISALNUM isalnum
    #endif

    #if !defined(TM_STRLEN)
        #include <string.h>
        #define TM_STRLEN strlen
    #endif
#endif /* defined(TM_CLI_IMPLEMENTATION) */

#ifndef _TM_CLI_H_INCLUDED_7B17B815_63AF_4D95_B74B_B457740DFB9C_
#define _TM_CLI_H_INCLUDED_7B17B815_63AF_4D95_B74B_B457740DFB9C_

#include "../common/tm_bool.inc"

#include "../common/tm_size_t.inc"

#include "../common/tm_null.inc"

/* Fixed width ints. Include C version so identifiers are in global namespace. */
#include <stdint.h>

/* For error logging. */
#include <cstdio>

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

/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

enum tmcli_argument_type {
    /* argument types */
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

struct tmcli_multiple {
    int max_amount; /* Set to a negative number if there is no maximum. */
    unsigned* counter;
};

typedef tm_bool (*tmcli_validator_type)(const char*);

#define CLI_ARGUMENT_STRING TM_NULL
#define CLI_NO_VALIDATOR TM_NULL
TMCLI_DEF tm_bool CLI_ARGUMENT_INT(const char* str);
TMCLI_DEF tm_bool CLI_ARGUMENT_UINT(const char* str);
TMCLI_DEF tm_bool CLI_ARGUMENT_FLOAT(const char* str);

typedef struct tmcli_option_struct {
    const char* short_option;
    const char* long_option;
    int argument_type;
    int option_type;

    tmcli_multiple multiple;
    tmcli_validator_type argument_validator;

    /* For options that have outputs, like options that can be multiple or flags */
    int output_type;
    unsigned* output;
    unsigned output_value;
} tmcli_option;

typedef struct tmcli_parser_settings_struct {
    FILE* error_log;              /* Error printing will be done into this file. */
    tm_bool allow_free_arguments; /* Whether to allow free arguments that aren't tied to an option, like "input.txt" */
} tmcli_parser_settings;

typedef struct tmcli_parser_struct {
    int argc;
    char const** argv;
    const tmcli_option* options;
    tm_size_t options_count;

    int current;

    uint32_t given_options[TMCLI_GIVEN_OPTIONS_COUNT];
    const char* current_arg;

    tmcli_parser_settings settings;
    const char* program_name;
    tm_bool error;
} tmcli_parser;

TMCLI_DEF tmcli_parser_settings tmcli_default_parser_settings();

/*
Creates a cli parser from argc and argv from the main function.
The resulting cli parser uses the default settings returned by tmcli_default_parser_settings.
*/
TMCLI_DEF tmcli_parser tmcli_make_parser(int argc, char const** argv, const tmcli_option* options,
                                         tm_size_t options_count);

/*
Creates a cli parser from given command line arguments and options.
Note that argv[0] must represent the very first command line argument, not the program name.
To call this function from arguments from main directly, you would have to call it like this:
    tmcli_make_parser_ex(argv[0], argc - 1, argv + 1, ...)
See implementation of tmcli_make_parser for how to call this function from argc/argv you get from main.
*/
TMCLI_DEF tmcli_parser tmcli_make_parser_ex(const char* program_name, int argc, char const** argv,
                                            const tmcli_option* options, tm_size_t options_count,
                                            tmcli_parser_settings_struct settings);

typedef struct tmcli_parsed_option_struct {
    const tmcli_option* option; /* Can be NULL if free arguments are allowed. */
    const char* argument;
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
#include "implementation.c"
#endif /* defined(TM_CLI_IMPLEMENTATION) */

#include "../common/LICENSE.inc"
