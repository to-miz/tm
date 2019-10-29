import os

def process_file(filename, tests):
    with open(filename, 'rb') as opened_file:
        data = opened_file.read()
        data_str = ""
        for byte in data:
            if byte < 127:
                c = chr(byte);
                if c == '"':
                    data_str += "\\\""
                elif c == '\\':
                    data_str += "\\\\"
                elif c == '\n':
                    data_str += "\\n"
                elif byte < 32:
                    data_str += "\\x{:X}\"\"".format(byte)
                else:
                    data_str += c
            else:
                data_str += "\\x{:X}\"\"".format(byte)
        ignore_str = " * doctest::may_fail(true)" if tests[0][0] else ""
        print('TEST_CASE("{}"{}) {{'.format(os.path.splitext(os.path.split(file)[1])[0], ignore_str))
        print('    const char* json = "{}";'.format(data_str))
        for ignore, expected, ex, flag in tests:
            result_str = "true" if expected else "false"
            ex_str = "_ex" if ex else ""
            print('    CHECK(check_json{}(json, {}, {}) == {});'.format(ex_str, len(data), flag, result_str))
        print('}')

for file in os.listdir("../../external/data/test_parsing"):
    if file.endswith(".json"):
        if file == "n_structure_100000_opening_arrays.json" or file == "n_structure_open_array_object.json":
            # These two tests are implemented manually because the json files are too big for a generated source file.
            continue
        if ("UTF16" in file) or ("utf16" in file) or ("UTF-16" in file) or ("BOM" in file) or ("UTF8_surrogate" in file):
            # Encodings other than Utf-8 not supported.
            continue;
        ignore = file.startswith("i")
        expected = file.startswith("y") or (ignore and ("surrogate" not in file))
        concat_file = os.path.join("../../external/data/test_parsing/", file)
        tests = [[ignore, expected, False, "JSON_READER_STRICT"]]
        if expected:
            tests.append([ignore, expected, False, "JSON_READER_JSON5"])
        process_file(concat_file, tests)

# from https://stackoverflow.com/a/19309964
files = [os.path.join(dp, f) for dp, dn, fn in os.walk("../../external/data/json5-tests") for f in fn]
for file in files:
    ignore = file.find("unicode") != -1
    if file.endswith(".json5"):
        process_file(file, [[ignore, False, False, "JSON_READER_STRICT"], [ignore, True, True, "JSON_READER_JSON5"]])
    elif file.endswith(".json"):
        process_file(file, [[ignore, True, False, "JSON_READER_STRICT"], [ignore, True, True, "JSON_READER_JSON5"]])
    elif file.endswith(".js"):
        process_file(file, [[ignore, False, False, "JSON_READER_STRICT"], [ignore, False, False, "JSON_READER_JSON5"]])
    elif file.endswith(".txt"):
        process_file(file, [[ignore, False, False, "JSON_READER_STRICT"], [ignore, False, False, "JSON_READER_JSON5"]])