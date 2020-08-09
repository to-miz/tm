#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <cwchar>

#ifdef TEXT
#undef TEXT
#endif

#define MAIN wmain
#define TEXT(x) L##x
#define TEXTLEN wcslen
#define TEXTCMP wcscmp

#define TMCLI_USE_WCHAR_T
#define FPRINTF fwprintf
#define FOPEN _wfopen
#define TSTRF L"%ls"
#define ASTRF L"%hs"
#define TSTRING std::wstring

#define IS_WCHAR 1
typedef wchar_t tchar;

#else /* !_WIN32 */

#define MAIN main
#define TEXT(x) x
#define TEXTLEN strlen
#define TEXTCMP strcmp

#define FPRINTF fprintf
#define FOPEN fopen
#define TSTRF "%s"
#define ASTRF "%s"
#define TSTRING std::string

#define IS_WCHAR 0
typedef char tchar;

#endif /* !_WIN32 */

// Use tm_cli.h for commandline parsing.
#define TM_CLI_IMPLEMENTATION
#include <tm_cli.h>

const tchar* app_name = nullptr;

void print_error(const tchar* app, const tchar* filename, const tchar* msg, int errnum) {
    if (errnum != 0) {
        FPRINTF(stderr, TSTRF TEXT(": ") TSTRF TEXT(" \"") TSTRF TEXT("\": ") ASTRF TEXT(".\n"), app, msg, filename,
                strerror(errnum));
    } else {
        FPRINTF(stderr, TSTRF TEXT(": ") TSTRF TEXT(" \"") TSTRF TEXT("\".\n"), app, msg, filename);
    }
}
void print_error(const tchar* app, const tchar* filename, const tchar* msg) { print_error(app, filename, msg, errno); }

bool close_stream(const tchar* app, const tchar* filename, const tchar* msg, FILE* stream) {
    errno = 0;
    fflush(stream);
    bool prev_error = ferror(stream) != 0;
    int ferror_errno = errno;
    bool close_error = fclose(stream) != 0;
    if (prev_error || close_error) {
        int errnum = (errno != 0) ? errno : ferror_errno;
        print_error(app, filename, msg, errnum);
        return false;
    }
    return true;
}
void print_cli_error(const tchar* error, const tchar* app) {
    FPRINTF(stdout, TSTRF TEXT(". Run \"") TSTRF TEXT(" --help\" for usage information.\n"), error, app);
}
void print_cli_command_error(const tchar* command, const tchar* app) {
    FPRINTF(stdout, TEXT("Run \"") TSTRF TEXT(" ") TSTRF TEXT(" --help\" for usage information.\n"), app, command);
}

#define FLAGS_TEXT                                                                                              \
    TEXT("  -f --flags                A comma seperated list (no spaces) of flags of what data to generate.\n") \
    TEXT("                            Valid flags are:\n")                                                      \
    TEXT("                                case_info\n")                                                         \
    TEXT("                                category\n")                                                          \
    TEXT("                                grapheme_break\n")                                                    \
    TEXT("                                width\n")                                                             \
    TEXT("                                canonical\n")                                                         \
    TEXT("                                compatibility\n")                                                     \
    TEXT("                                full_case\n")                                                         \
    TEXT("                                full_case_fold\n")                                                    \
    TEXT("                                full_case_toggle\n")                                                  \
    TEXT("                                simple_case\n")                                                       \
    TEXT("                                simple_case_fold\n")                                                  \
    TEXT("                                simple_case_toggle\n")                                                \
    TEXT("                                prune_stage_one\n")                                                   \
    TEXT("                                prune_stage_two\n")                                                   \
    TEXT("                                handle_invalid_codepoints\n")                                         \
    TEXT("                                default\n")

#define GENERIC_OPTIONS_TEXT                                                                                         \
    TEXT("  -p --prefix               What prefix to use for the generated functions and tables (ascii only).\n")    \
    TEXT("  --assert                  Assert function name to use (ascii only).\n")                                  \
    TEXT("  --no-assert               Do not generate assert calls.\n")                                              \
    TEXT("  -o --output               Path to generated output source file. If not specified, outputs to stdout.\n") \
    TEXT("  --header                  Path to generated output header file. If not specified, outputs to stdout.\n") \
    TEXT("  --no-header-guard         Do not generate header guard.\n")                                              \
    TEXT("  -h --help                 Show this screen.\n")

// clang-format off
#define GENERIC_OPTIONS                                                     \
    {TEXT("h"), TEXT("help")},                                              \
    {TEXT("o"), TEXT("output"), CLI_REQUIRED_ARGUMENT},                     \
    {nullptr, TEXT("header"), CLI_REQUIRED_ARGUMENT},                       \
    {nullptr, TEXT("no-header-guard"), CLI_NO_ARGUMENT},                    \
    {TEXT("f"), TEXT("flags"), CLI_REQUIRED_ARGUMENT, CLI_OPTIONAL_OPTION}, \
    {TEXT("p"), TEXT("prefix"), CLI_REQUIRED_ARGUMENT},                     \
    {nullptr, TEXT("assert"), CLI_REQUIRED_ARGUMENT},                       \
    {nullptr, TEXT("no-assert"), CLI_NO_ARGUMENT},
// clang-format on

void print_usage(const tchar* app) {
    // clang-format off
    const tchar* usage =
        TEXT("Usage:\n")
        TEXT("  ") TSTRF TEXT(" dir <root_dir> [-f<flags>] [-o<out_file>]\n")
        TEXT("  ") TSTRF TEXT(" files <path_to/UnicodeData.txt> <path_to/CaseFolding.txt> <path_to/SpecialCasing.txt>\n")
        TEXT("    <path_to/GraphemeBreakProperty.txt> <path_to/emoji-data.txt> [-f<flags>] [-o<out_file>]\n")
        TEXT("  ") TSTRF TEXT(" -h | --help\n")
        TEXT("  ") TSTRF TEXT(" --version\n")
        TEXT("\n")
        TEXT("Commands:\n")
        TEXT("  dir                       Search for UnicodeData.txt, CaseFolding.txt, SpecialCasing.txt and\n")
        TEXT("                            GraphemeBreakProperty.txt in <root_dir>.\n")
        TEXT("  files                     Take explicit paths via options.\n")
        TEXT("\n")
        TEXT("Options:\n")
        FLAGS_TEXT
        GENERIC_OPTIONS_TEXT
        TEXT("  --version                 Show version.\n")
        TEXT("\n")
        TEXT("This tool generates a C source file with a 2-stage lookup table for unicode codepoints.\n");
    // clang-format on
    FPRINTF(stdout, usage, app, app, app, app);
}

void print_files_usage() {
    // clang-format off
    const tchar* usage =
        TEXT("Usage:\n")
        TEXT("  ") TSTRF TEXT(" files -u<path_to/UnicodeData.txt> -c<path_to/CaseFolding.txt>\n")
        TEXT("           -s<path_to/SpecialCasing.txt> -g<path_to/GraphemeBreakProperty.txt>\n")
        TEXT("           -a<path_to/EastAsianWidth.txt> -e<path_to/emoji-data.txt>\n")
        TEXT("           [-t<path_to/GraphemeBreakTest.txt>] [-f<flags>] [-o<out_file>]\n")
        TEXT("  ") TSTRF TEXT(" files -h | --help\n")
        TEXT("\n")
        TEXT("Options:\n")
        TEXT("  -u --unicode_data         Path to UnicodeData.txt.\n")
        TEXT("  -c --case_folding         Path to CaseFolding.txt.\n")
        TEXT("  -s --special_casing       Path to SpecialCasing.txt.\n")
        TEXT("  -g --grapheme_break       Path to GraphemeBreakProperty.txt.\n")
        TEXT("  -a --east_asian_width     Path to EastAsianWidth.txt.\n")
        TEXT("  -e --emoji_data           Path to emoji_data.txt.\n")
        TEXT("  -t --grapheme_break_test  Path to GraphemeBreakTest.txt. Optional.\n")
        FLAGS_TEXT
        GENERIC_OPTIONS_TEXT
        TEXT("\n")
        TEXT("The files command takes explicit paths via options.\n");
    // clang-format on
    FPRINTF(stdout, usage, app_name, app_name);
}

void print_dir_usage() {
    // clang-format off
    const tchar* usage =
        TEXT("Usage:\n")
        TEXT("  ") TSTRF TEXT(" dir <root_dir> [-f<flags>] [-o<out_file>]\n")
        TEXT("  ") TSTRF TEXT(" dir -h | --help\n")
        TEXT("\n")
        TEXT("Arguments:\n")
        TEXT("  root_dir                  Directory in which to look for UnicodeData.txt, CaseFolding.txt,\n")
        TEXT("                            SpecialCasing.txt, EastAsianWidth.txt and GraphemeBreakProperty.txt.\n")
        TEXT("                            If GraphemeBreakTest.txt also exists, it will be automatically used.\n")
        TEXT("Options:\n")
        FLAGS_TEXT
        GENERIC_OPTIONS_TEXT
        TEXT("\n")
        TEXT("The files command takes explicit paths via options.\n");
    // clang-format on
    FPRINTF(stdout, usage, app_name, app_name);
}

void print_version() {
    FPRINTF(stdout, TEXT("unicode_gen version %X.%X.%X\n"), VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
}

void close_stdout() {
    if (!close_stream(app_name, TEXT("stdout"), TEXT("Error writing to"), stdout)) exit(-1);
}

struct option_strings {
    char prefix[255] = {};
    char uppercase_prefix[255] = {};
    char assert_name[255] = "assert";
};

struct parsed_options {
    TSTRING unicode_data_filename;
    TSTRING case_folding_filename;
    TSTRING special_casing_filename;
    TSTRING grapheme_break_filename;
    TSTRING emoji_data_filename;
    TSTRING east_asian_width_filename;
    TSTRING grapheme_break_test_filename;
    const tchar* out_filename = nullptr;
    const tchar* header_filename = nullptr;

    uint32_t flags = generate_flags_default;

    option_strings strings;
};

TSTRING concat_path(const tchar* path, const tchar* filename) {
    auto path_len = TEXTLEN(path);
    auto filename_len = TEXTLEN(filename);
    TSTRING result;
    result.reserve(path_len + filename_len + 1);
    result.assign(path, path + path_len);
    if (path_len && (result.back() != TEXT('/')
#ifdef _WIN32
                     || result.back() != TEXT('\\')
#endif
                         )) {
        result.push_back(TEXT('/'));
    }
    result.insert(result.end(), filename, filename + filename_len);
    return result;
}

bool copy_as_ascii_string_dumb(const tchar* str, char* out, size_t out_len) {
    auto remaining = out_len;
    while (*str) {
        auto c = *str++;
#if IS_WCHAR
        if ((char)c < 0 || c > 127) {
            FPRINTF(stderr, TEXT("Prefix is not ascii.\n"));
            return false;
        }
#endif
        if (remaining == 0) {
            FPRINTF(stderr, TEXT("Prefix too long (> %zu).\n"), out_len);
            return false;
        }
        *out++ = (char)c;
        --remaining;
    }
    if (remaining == 0) {
        FPRINTF(stderr, TEXT("Prefix too long (> %zu).\n"), out_len);
        return false;
    }
    *out = 0;
    return true;
}

bool to_generate_flags(const tchar* str, uint32_t* out) {
    assert(out);

    struct flags_pair {
        const tchar* name;
        uint32_t value;
    };
    static const flags_pair generate_flags_enum_pairs[] = {
        {TEXT("case_info"), generate_flags_case_info},
        {TEXT("category"), generate_flags_category},
        {TEXT("grapheme_break"), generate_flags_grapheme_break},
        {TEXT("width"), generate_flags_width},
        {TEXT("canonical"), generate_flags_canonical},
        {TEXT("compatibility"), generate_flags_compatibility},
        {TEXT("full_case"), generate_flags_full_case},
        {TEXT("full_case_fold"), generate_flags_full_case_fold},
        {TEXT("full_case_toggle"), generate_flags_full_case_toggle},
        {TEXT("simple_case"), generate_flags_simple_case},
        {TEXT("simple_case_fold"), generate_flags_simple_case_fold},
        {TEXT("simple_case_toggle"), generate_flags_simple_case_toggle},
        {TEXT("prune_stage_one"), generate_flags_prune_stage_one},
        {TEXT("prune_stage_two"), generate_flags_prune_stage_two},
        {TEXT("handle_invalid_codepoints"), generate_flags_handle_invalid_codepoints},
        {TEXT("default"), generate_flags_default},
    };

    // Mask out all the flags that are in the array above.
    *out = ((*out) & (~generate_flags_named_flags_mask));
    for (const tchar* p = str;; p++) {
        if (*p == TEXT(',') || *p == 0) {
            auto str_len = (size_t)(p - str);
            if (!str_len) {
                if (*p == 0) break;
                continue;
            }

            bool found = false;
            for (size_t i = 0, count = size(generate_flags_enum_pairs); i < count; i++) {
                assert(p >= str);
                auto name_len = TEXTLEN(generate_flags_enum_pairs[i].name);
                if (str_len != name_len) continue;
                if (memcmp(str, generate_flags_enum_pairs[i].name, str_len * sizeof(tchar)) == 0) {
                    *out |= generate_flags_enum_pairs[i].value;
                    found = true;
                    break;
                }
            }
            if (!found) return false;
            if (*p == 0) break;
            str = p + 1;
        }
    }
    return true;
}

void handle_generic_options(const tchar* command, const tmcli_parsed_option& option, void (*print_usage)(),
                            parsed_options* out) {
    switch (option.option_index) {
        case 0: {
            print_usage();
            exit(0);
            break;
        }
        case 1: {
            out->out_filename = option.argument;
            break;
        }
        case 2: {
            out->header_filename = option.argument;
            break;
        }
        case 3: {
            out->flags |= generate_flags_no_header_guard;
            break;
        }
        case 4: {
            if (!to_generate_flags(option.argument, &out->flags)) {
                FPRINTF(stderr, TEXT("Unknown flags.\n"));
                print_cli_command_error(command, app_name);
                exit(-1);
            }
            break;
        }
        case 5: {
            if (!copy_as_ascii_string_dumb(option.argument, out->strings.prefix, size(out->strings.prefix))) {
                print_cli_command_error(command, app_name);
                exit(-1);
            }
            {
                auto pre = out->strings.prefix;
                char* p = out->strings.uppercase_prefix;
                const char* last = out->strings.uppercase_prefix + size(out->strings.uppercase_prefix);
                while (*pre && p < last) *p++ = (char)toupper((unsigned char)*pre++);
                if (p < last) {
                    *p = 0;
                } else {
                    *(p - 1) = 0;
                }
            }
            break;
        }
        case 6: {
            if (!copy_as_ascii_string_dumb(option.argument, out->strings.assert_name, size(out->strings.assert_name))) {
                print_cli_command_error(command, app_name);
                exit(-1);
            }
            break;
        }
        case 7: {
            out->flags |= generate_flags_no_assert;
            break;
        }
    }
}

parsed_options parse_dir(int argc, tchar const* argv[]) {
    static const tmcli_option options[] = {GENERIC_OPTIONS};

    parsed_options result = {};

    tmcli_parser_settings settings = {stderr, /*allow_free_arguments=*/true};
    auto parser = tmcli_make_parser_ex(app_name, argc, argv, options, size(options), settings);

    bool dir_specified = false;
    tmcli_parsed_option parsed_option = {};
    while (tmcli_next(&parser, &parsed_option)) {
        if (!parsed_option.option) {
            if (dir_specified) {
                FPRINTF(stderr, TEXT("Too many directories specified.\n"));
                print_cli_command_error(TEXT("dir"), app_name);
                exit(-1);
            }
            auto root_dir = parsed_option.argument;
            result.unicode_data_filename = concat_path(root_dir, TEXT("UnicodeData.txt"));
            result.case_folding_filename = concat_path(root_dir, TEXT("CaseFolding.txt"));
            result.special_casing_filename = concat_path(root_dir, TEXT("SpecialCasing.txt"));
            result.grapheme_break_filename = concat_path(root_dir, TEXT("GraphemeBreakProperty.txt"));
            result.emoji_data_filename = concat_path(root_dir, TEXT("emoji-data.txt"));
            result.east_asian_width_filename = concat_path(root_dir, TEXT("EastAsianWidth.txt"));
            result.grapheme_break_test_filename = concat_path(root_dir, TEXT("GraphemeBreakTest.txt"));
            dir_specified = true;
            continue;
        }
        handle_generic_options(TEXT("dir"), parsed_option, print_dir_usage, &result);
    }
    if (!dir_specified) {
        FPRINTF(stderr, TEXT("No directories specified.\n"));
        print_cli_command_error(TEXT("dir"), app_name);
        exit(-1);
    }
    if (!tmcli_validate(&parser)) {
        print_cli_command_error(TEXT("files"), app_name);
        exit(-1);
    }

    return result;
}

parsed_options parse_files(int argc, tchar const* argv[]) {
    static const tmcli_option options[] = {
        {TEXT("u"), TEXT("unicode_data"), CLI_REQUIRED_ARGUMENT, CLI_REQUIRED_OPTION},
        {TEXT("c"), TEXT("case_folding"), CLI_REQUIRED_ARGUMENT, CLI_REQUIRED_OPTION},
        {TEXT("s"), TEXT("special_casing"), CLI_REQUIRED_ARGUMENT, CLI_REQUIRED_OPTION},
        {TEXT("g"), TEXT("grapheme_break"), CLI_REQUIRED_ARGUMENT, CLI_REQUIRED_OPTION},
        {TEXT("a"), TEXT("east_asian_width"), CLI_REQUIRED_ARGUMENT, CLI_REQUIRED_OPTION},
        {TEXT("e"), TEXT("emoji_data"), CLI_REQUIRED_ARGUMENT, CLI_REQUIRED_OPTION},

        {nullptr, TEXT("grapheme_break_test"), CLI_REQUIRED_ARGUMENT},
        GENERIC_OPTIONS};

    parsed_options result = {};

    tmcli_parser_settings settings = {stderr, /*allow_free_arguments=*/false};
    auto parser = tmcli_make_parser_ex(app_name, argc, argv, options, size(options), settings);

    tmcli_parsed_option parsed_option = {};
    while (tmcli_next(&parser, &parsed_option)) {
        if (!parsed_option.option) break;
        switch (parsed_option.option_index) {
            case 0: {
                result.unicode_data_filename = parsed_option.argument;
                break;
            }
            case 1: {
                result.case_folding_filename = parsed_option.argument;
                break;
            }
            case 2: {
                result.special_casing_filename = parsed_option.argument;
                break;
            }
            case 3: {
                result.grapheme_break_filename = parsed_option.argument;
                break;
            }
            case 4: {
                result.emoji_data_filename = parsed_option.argument;
                break;
            }
            case 5: {
                result.east_asian_width_filename = parsed_option.argument;
                break;
            }
            case 6: {
                result.grapheme_break_test_filename = parsed_option.argument;
                break;
            }
            case 7: {
                result.out_filename = parsed_option.argument;
                break;
            }
            default: {
                parsed_option.option_index -= 7;
                handle_generic_options(TEXT("files"), parsed_option, print_files_usage, &result);
                break;
            }
        }
    }
    if (!tmcli_validate(&parser)) {
        print_cli_command_error(TEXT("files"), app_name);
        exit(-1);
    }

    return result;
}

parsed_options parse_cli(int argc, tchar const* argv[]) {
    if (argc <= 1) {
        print_usage(app_name);
        exit(-1);
    }

    if (TEXTCMP(argv[1], TEXT("-h")) == 0 || TEXTCMP(argv[1], TEXT("--help")) == 0) {
        print_usage(app_name);
        exit(0);
    }

    if (TEXTCMP(argv[1], TEXT("--version")) == 0) {
        print_version();
        exit(0);
    }

    if (TEXTCMP(argv[1], TEXT("dir")) == 0) {
        return parse_dir(argc - 2, argv + 2);
    } else if (TEXTCMP(argv[1], TEXT("files")) == 0) {
        return parse_files(argc - 2, argv + 2);
    } else {
        print_cli_command_error(TEXT("Unknown command."), app_name);
        exit(-1);
    }
}