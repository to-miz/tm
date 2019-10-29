import os

for file in os.listdir("../../external/data/test_parsing"):
    if file.endswith(".json"):
        if file == "n_structure_100000_opening_arrays.json" or file == "n_structure_open_array_object.json":
            # These two tests are implemented manually because the json files are too big for a generated source file.
            continue
        if ("UTF16" in file) or ("utf16" in file) or ("UTF-16" in file) or ("BOM" in file) or ("UTF8_surrogate" in file):
            # Encodings other than Utf-8 not supported.
            continue;
        ignore = file.startswith("i")
        required = file.startswith("y") or (ignore and ("surrogate" not in file))
        concat_file = os.path.join("../../external/data/test_parsing/", file)
        with open(concat_file, 'rb') as opened_file:
            result_str = "true" if required else "false"
            ignore_str = " * doctest::may_fail(true)" if ignore else ""
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
            print(
"""TEST_CASE("{}"{}) {{
    const char* json = "{}";
    REQUIRE(check_json(json, {}, JSON_READER_STRICT) == {});
}}
""".format(file[:-5], ignore_str, data_str, len(data), result_str)
            )