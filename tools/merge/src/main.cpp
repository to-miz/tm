#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

struct commandline_params {
    const char* in;
    const char* out;
    const char* dir;
    bool recursive;
    bool valid;
};

commandline_params parse_commandline(int argc, char const* argv[]) {
    commandline_params result = {};
    if (argc < 4) {
        return result;
    }
    result.in = argv[1];
    result.out = argv[2];
    result.dir = argv[3];
    if (argc >= 5) {
        if (strcmp(argv[4], "-r") == 0 || strcmp(argv[4], "--recursive") == 0) {
            result.recursive = true;
        }
    }
    result.valid = true;
    return result;
}

struct read_file_result {
    std::vector<char> data;
    bool success;
};

read_file_result read_file(const char* filename, bool nullterminate = true) {
    read_file_result result = {};
    if (auto file = fopen(filename, "r")) {
        char buf[1024];
        while (size_t len = fread(buf, 1, sizeof(buf) / sizeof(char), file)) {
            result.data.insert(result.data.end(), buf, buf + len);
        }
        if (nullterminate) result.data.push_back(0);
        fclose(file);
        result.success = true;
    }
    return result;
}

bool write_file(const char* filename, const std::vector<char> data) {
    if (auto file = fopen(filename, "w")) {
        auto size = fwrite(data.data(), 1, data.size(), file);
        auto success = size == data.size();
        fclose(file);
        return success;
    }
    return false;
}

enum token_type { tok_hash, tok_identifier, tok_string, tok_eof };

struct token_t {
    token_type type;
    int len;
    const char* str;
};

struct stream_t {
    const char* p;
    const char* end;

    int get_char() const { return (unsigned char)*p; }
};

const char* find(const char* str, int c, size_t len) { return (const char*)std::memchr(str, c, len); }

void skip_whitespace(stream_t* stream) {
    while (stream->get_char() && find(" \t\r\n\v\f", stream->get_char(), 6)) {
        ++stream->p;
    }
}

int distance(const char* a, const char* b) {
    assert(b >= a);
    auto result = b - a;
    assert(result >= 0);
    assert(result < INT32_MAX);
    return (int)result;
}

token_t next_token(stream_t* stream) {
    skip_whitespace(stream);
    auto p = stream->p;
    const auto end = stream->end;

    const auto eof = token_t{tok_eof, distance(stream->p, stream->end), stream->end};
    for (;;) {
        switch (*p) {
            case '/': {
                ++p;
                if (*p == '*') {
                    // skip block comment
                    ++p;
                    for (;;) {
                        auto potential = find(p, '*', end - p);
                        if (!potential) {
                            return eof;
                        }
                        if (*(potential + 1) == '/') {
                            p = potential + 2;
                            break;
                        }
                        p = potential + 1;
                    }
                } else if (*p == '/') {
                    // skip line comment
                    auto potential = find(p, '\n', end - p);
                    if (!potential) {
                        return eof;
                    }
                    p = potential + 1;
                }
                break;
            }
            case '#': {
                auto pos = p;
                ++p;
                stream->p = p;
                return {tok_hash, 1, pos};
            }
            case 0:
                return eof;
            case '"': {
                ++p;
                auto start = p;
                for (;;) {
                    auto potential = find(p, '"', end - p);
                    if (!potential) {
                        return eof;
                    }
                    if (*potential == '"' && *(potential - 1) != '\\') {
                        p = potential + 1;
                        stream->p = p;
                        auto len = distance(start, potential);
                        assert(len >= 0);
                        return {tok_string, len, start};
                    }
                    p = potential + 1;
                }
            }
            default: {
                // identifier
                auto start = p;
                while (isalnum((unsigned char)*p) || *p == '_') {
                    ++p;
                }
                if (p == start) {
                    ++p;
                }
                stream->p = p;
                return {tok_identifier, distance(start, p), start};
            }
        }
    }
}

std::string concat_paths(const char* path1, size_t path1len, const std::string& path2) {
    auto result = std::string(path1, path1len);
    if (!result.empty() && result.back() != '/' && result.back() != '\\') {
        result.push_back('/');
    }
    result += path2;
    return result;
}

struct stream_guard {
   private:
    stream_t guard;
    stream_t* stream;
    bool engaged = true;

   public:
    stream_guard(stream_t* stream) : guard(*stream), stream(stream) {}
    ~stream_guard() {
        if (engaged) {
            assert(stream);
            *stream = guard;
        }
    }
    void dismiss() { engaged = false; }
};

enum merge_error_t { merge_ok, merge_skip, merge_error };

struct merge_definition {
    const char* name;
    const char* value;
    int name_len;
    int value_len;
};

merge_error_t parse_define_statement(stream_t* stream, std::vector<merge_definition>& out) {
    stream_guard guard(stream);

    auto token = next_token(stream);
    if (token.type != tok_identifier || strncmp(token.str, "define", token.len) != 0) {
        return merge_skip;
    }
    token = next_token(stream);
    if (token.type != tok_identifier || strncmp(token.str, "MERGE_", 6) != 0) {
        return merge_skip;
    }
    skip_whitespace(stream);
    if (*stream->p == '(') {
        fprintf(stderr, "Merge definitions can't be macros.\n");
        return merge_error;
    }

    auto line_end = find(stream->p, '\n', (size_t)(stream->end - stream->p));
    if (!line_end) {
        line_end = stream->end;
    }
    out.push_back({token.str, stream->p, token.len, distance(stream->p, line_end)});
    stream->p = line_end;
    guard.dismiss();
    return merge_ok;
}

void erase_undef_statements(std::vector<char>& data, const std::vector<merge_definition>& definitions) {
    for (auto& def : definitions) {
        auto pos = data.begin();

        std::string search_val = "#undef ";
        search_val.insert(search_val.end(), def.name, def.name + def.name_len);

        for (;;) {
            pos = std::search(pos, data.end(), search_val.begin(), search_val.end());
            if (pos == data.end()) {
                break;
            }
            auto last = pos + search_val.size();
            // only replace if we matched to a whole identifier and not a substr
            if ((pos != data.begin() && isalnum(*(pos - 1))) || (last + 1 != data.end() && isalnum(*(last + 1)))) {
                pos = last;
                continue;
            }
            // extend range to end of line
            last = std::find(last, data.end(), '\n');
            if (last != data.end()) {
                ++last;
            }
            pos = data.erase(pos, last);
        }
    }
}
struct merge_result {
    std::vector<char> data;
    std::vector<std::string> merged_files;
    bool success;
    bool changed;
};

struct indentation_result {
    int amount;
    bool use_spaces;
};

indentation_result get_indent(std::vector<char>::iterator first, std::vector<char>::iterator last) {
    indentation_result result = {};
    if (first != last) {
        auto spaces = 0;
        --last;
        for (auto it = last; it != first; --it) {
            auto c = *it;
            if (c == '\t') {
                ++result.amount;
            } else if (c == ' ') {
                result.use_spaces = true;
                ++spaces;
                if (spaces == 4) {
                    ++result.amount;
                    spaces = 0;
                }
            } else {
                break;
            }
        }
	}
    return result;
}
void indent_range(std::vector<char>& data, std::vector<char>::iterator first, std::vector<char>::iterator last,
                  indentation_result indentation) {
    if (indentation.amount == 0) {
        return;
    }
    // indent parsed lines based on indentation level of the directive

    const char* indent_spaces = "    ";
    const char* indent_tabs = "\t";
    const char* indent_chars_begin = nullptr;
    const char* indent_chars_end = nullptr;

    if (indentation.use_spaces) {
        indent_chars_begin = indent_spaces;
        indent_chars_end = indent_spaces + 4;
    } else {
        indent_chars_begin = indent_tabs;
        indent_chars_end = indent_tabs + 1;
    }

    // Skip first line, since it already is indented.
    first = std::find(first, last, '\n');
    while (first != last && (*first == '\r' || *first == '\n')) {
        ++first;
    }
    if (first == last) return;
    for (;;) {
        auto dist = std::distance(first, last);
        for (auto i = 0; i < indentation.amount; ++i) {
            first = data.insert(first, indent_chars_begin, indent_chars_end);
            first += std::distance(indent_chars_begin, indent_chars_end);
        }
        last = first + dist;
        first = std::find(first, last, '\n');
        while (first != last && (*first == '\r' || *first == '\n')) {
            ++first;
        }
        if (first == last) return;
    }
}

merge_error_t merge_include_statement(stream_t* stream, const char* dir, size_t dir_len,
                                      const std::vector<merge_definition>& definitions,
                                      const std::vector<std::string>& already_included, merge_result& out) {
    stream_guard guard(stream);

    auto token = next_token(stream);
    if (token.type != tok_identifier || strncmp(token.str, "include", token.len) != 0) {
        return merge_skip;
    }
    token = next_token(stream);
    if (token.type != tok_string) {
        return merge_skip;
    }

    // TODO: Use hashes or hashmap maybe.
    auto name = std::string(token.str, token.len);
    if (std::find(already_included.begin(), already_included.end(), name) != already_included.end()) {
        fprintf(stderr, "Potential circular dependency detected: %s.\nUse flat hiearchies.\n", name.c_str());
        return merge_error;
    }

    auto filename = concat_paths(dir, dir_len, name);
    if (filename.empty()) {
        fprintf(stderr, "Include path is empty.\n");
        return merge_error;
    }

    auto file = read_file(filename.c_str(), /*nullterminate=*/false);
    if (!file.success) {
        // If we can't find the file, assume that it wasn't meant for merging.
        return merge_skip;
    }

    erase_undef_statements(file.data, definitions);

    for (auto& def : definitions) {
        // replace all occurences of def.name with def.value
        auto pos = file.data.begin();
        for (;;) {
            pos = std::search(pos, file.data.end(), def.name, def.name + def.name_len);
            if (pos == file.data.end()) {
                break;
            }
            auto last = pos + def.name_len;
            // only replace if we matched to a whole identifier and not a substr
            if ((pos != file.data.begin() && isalnum(*(pos - 1))) ||
                (last + 1 != file.data.end() && isalnum(*(last + 1)))) {
                pos = last;
                continue;
            }
            pos = file.data.erase(pos, last);
            pos = file.data.insert(pos, def.value, def.value + def.value_len);
            pos += def.value_len;
        }
    }
    if (std::find(out.merged_files.begin(), out.merged_files.end(), name) == out.merged_files.end()) {
        out.merged_files.emplace_back(std::move(name));
    }
    auto indentation = get_indent(out.data.begin(), out.data.end());
    indent_range(file.data, file.data.begin(), file.data.end(), indentation);
    out.data.insert(out.data.end(), file.data.begin(), file.data.end());
    guard.dismiss();
    return merge_ok;
}

merge_result merge(const std::vector<char>& in, const char* dir, const std::vector<std::string>& already_included) {
    assert(in.size());
    assert(in[in.size() - 1] == 0);

    auto stream = stream_t{in.data(), in.data() + in.size()};
    const auto dir_len = strlen(dir);

    merge_result result = {};
    if (dir_len > 260) {
        return result;
    }

    result.data.reserve((size_t)(stream.end - stream.p));

    std::vector<merge_definition> definitions;

    for (;;) {
        auto prev = stream.p;
        auto token = next_token(&stream);
        if (token.type != tok_eof && prev != token.str) {
            // output everything that comes before the token
            result.data.insert(result.data.end(), prev, token.str);
            prev = token.str;
        }
        switch (token.type) {
            case tok_eof: {
                result.data.insert(result.data.end(), prev, stream.end - 1);
                result.merged_files.insert(result.merged_files.end(), already_included.begin(), already_included.end());
                result.success = true;
                return result;
            }
            case tok_hash: {
                switch (merge_include_statement(&stream, dir, dir_len, definitions, already_included, result)) {
                    case merge_ok:
                        result.changed = true;
                        definitions.clear();
                        continue;
                    case merge_error:
                        return result;
                    default:
                        break;
                }

                switch (parse_define_statement(&stream, definitions)) {
                    case merge_ok:
                        continue;
                    case merge_error:
                        return result;
                    default:
                        break;
                }
            }
            default: { break; }
        }
        result.data.insert(result.data.end(), prev, stream.p);
    }
}

int main(int argc, char const* argv[]) {
    auto params = parse_commandline(argc, argv);
    if (!params.valid) {
        printf(
            "OVERVIEW: A tool to merge multiple C/C++ files into one.\n"
            "\n"
            "Parses #include \".*\" directives and merges contents into output file.\n"
            "Any #defines before such an include that starts with MERGE_ will be\n"
            "automatically expanded and #undef statements will be removed.\n"
            "\n"
            "USAGE: %s <in-filename> <out-filename> <include-dir>",
            argv[0]);
        return -1;
    }

    auto in = read_file(params.in);
    if (!in.success) {
        fprintf(stderr, "Can't read from %s\n", params.in);
        return -1;
    }

    merge_result result = {};
    do {
        result = merge(in.data, params.dir, result.merged_files);
        if (!result.success) {
            return -1;
        }
        if (params.recursive && result.changed) {
            in.data = std::move(result.data);
            in.data.push_back(0);
            result.data.clear();
        }
    } while (params.recursive && result.changed);

    if (!write_file(params.out, result.data)) {
        printf("Can't write to file %s\n", params.out);
        return -1;
    }
    return 0;
}