TEST_CASE("existence") {
    allocation_guard alloc_guard;
    mock.clear();
    mock.entries.push_back({"test_file", nullptr, mock_file});
    mock.entries.push_back({"test_dir/", nullptr, mock_dir});
    mock.entries.push_back({"test_dir/data", nullptr, mock_dir});

    CHECK(tmu_file_exists("test_file").exists);
    CHECK(tmu_directory_exists("test_dir/").exists);
    CHECK(tmu_directory_exists("test_dir/data").exists);

    CHECK(!tmu_file_exists("test_dir/asdasdasd.txt").exists);
    CHECK(!tmu_directory_exists("test_dir/asdasdsadsad/").exists);
    CHECK(!tmu_directory_exists("test_dir/asdasdsadsad").exists);

    REQUIRE(mock.entries.size() == 3);
    REQUIRE(!mock.file.is_open);
}

// Test string_view routes, since they should always go the allocation/sbo route.
TEST_CASE("string_view") {
    allocation_guard alloc_guard;
    mock.clear();
    mock.entries.push_back({"test_file", nullptr, mock_file});
    mock.entries.push_back({"test_dir/", nullptr, mock_dir});
    // This should allocate since path length > TMU_SBO_SIZE(15) size.
    CHECK(tmu_file_exists(string_view{"test_file"}).exists);

    // This should use sbo.
    CHECK(tmu_directory_exists(string_view{"test_dir/"}).exists);

    REQUIRE(mock.entries.size() == 2);
    REQUIRE(!mock.file.is_open);
}

TEST_CASE("timestamps") {
    allocation_guard alloc_guard;
    mock.clear();
    mock.entries.push_back({"test_file", nullptr, mock_file});

    CHECK(tmu_file_timestamp("test_file").ec == TM_OK);
    CHECK(tmu_file_timestamp("test_dir/asdasd.txt").ec != TM_OK);

    CHECK(tmu_compare_file_time(0, 1) < 0);
    CHECK(tmu_compare_file_time(0, 0) == 0);
    CHECK(tmu_compare_file_time(1, 0) > 0);

    REQUIRE(mock.entries.size() == 1);
    REQUIRE(!mock.file.is_open);
}

TEST_CASE("write and read") {
    const char* filename = "build/test.txt";

    allocation_guard alloc_guard;
    mock.clear();
    mock.entries.push_back({filename, nullptr, mock_file});

    const char* test_data = "tmu_test file data";
    tm_size_t test_data_len = (tm_size_t)strlen(test_data);

    auto write_result = tmu_write_file(filename, test_data, test_data_len);
    REQUIRE(write_result.ec == TM_OK);
    REQUIRE(write_result.written == test_data_len);
    REQUIRE(!mock.file.is_open);
    REQUIRE(mock.entries.size() == 1);
    REQUIRE((tm_size_t)mock.entries[0].contents.size() == write_result.written);

    auto read_result = tmu_read_file_managed(filename);
    REQUIRE(read_result.ec == TM_OK);
    REQUIRE(read_result.contents.size == test_data_len);
    REQUIRE(memcmp(read_result.contents.data, test_data, test_data_len) == 0);
    REQUIRE(!mock.file.is_open);

    auto vec = tmu_read_file_to_vector(filename);
    REQUIRE(!vec.empty());
    REQUIRE((tm_size_t)vec.size() == test_data_len);
    REQUIRE(memcmp(vec.data(), test_data, test_data_len) == 0);

    auto vec_utf8 = tmu_read_file_as_utf8_to_vector(filename);
    REQUIRE(!vec_utf8.empty());
    REQUIRE((tm_size_t)vec_utf8.size() == test_data_len);
    REQUIRE(memcmp(vec_utf8.data(), test_data, test_data_len) == 0);

    REQUIRE(tmu_delete_file(filename) == TM_OK);
    REQUIRE(mock.entries.empty());
}

TEST_CASE("renaming") {
    allocation_guard alloc_guard;
    mock.clear();

    const char* test_data = "tmu_test file data";
    tm_size_t test_data_len = (tm_size_t)strlen(test_data);
    const char* from = "build/test.txt";
    const char* to = "build/test_renamed.txt";

    // Create a file.
    auto write_result = tmu_write_file(from, test_data, test_data_len);
    REQUIRE(write_result.ec == TM_OK);
    REQUIRE(write_result.written == test_data_len);
    REQUIRE(mock.entries.size() == 1);

    // Rename the file.
    REQUIRE(tmu_rename_file_ex(from, to, tmu_overwrite) == TM_OK);
    REQUIRE(mock.entries.size() == 1);

    // Make sure old filename doesn't exist anymore.
    auto from_exists = tmu_file_exists(from);
    REQUIRE(from_exists.ec == TM_OK);
    REQUIRE(!from_exists.exists);

    // Create file again.
    write_result = tmu_write_file(from, test_data, test_data_len);
    REQUIRE(write_result.ec == TM_OK);
    REQUIRE(write_result.written == test_data_len);
    REQUIRE(mock.entries.size() == 2);

    // Overwrite files.
    REQUIRE(tmu_rename_file_ex(from, to, tmu_overwrite) == TM_OK);
    REQUIRE(mock.entries.size() == 1);

    // Make sure overwrite succeeded.
    from_exists = tmu_file_exists(from);
    REQUIRE(from_exists.ec == TM_OK);
    REQUIRE(!from_exists.exists);

    // Delete files.
    REQUIRE(tmu_delete_file(to) == TM_OK);
    REQUIRE(mock.entries.empty());

    // Make sure they don't exist.
    from_exists = tmu_file_exists(from);
    REQUIRE(from_exists.ec == TM_OK);
    REQUIRE(!from_exists.exists);

    auto to_exists = tmu_file_exists(to);
    REQUIRE(to_exists.ec == TM_OK);
    REQUIRE(!to_exists.exists);
}

string get_dir(const char* base, const char** dirs, size_t dirs_len) {
    string result = base;
    for (size_t i = 0; i < dirs_len; ++i) {
        result += "/";
        result += dirs[i];
    }
    return result;
}

TEST_CASE("directory tree") {
    allocation_guard alloc_guard;
    mock.clear();

    const char* base = "test";
    const char* dirs[] = {"a", "b", "c"};

    REQUIRE(mock.entries.empty());

    {
        // Create whole directory tree at once.
        auto dir = get_dir(base, dirs, size(dirs));
        REQUIRE(tmu_create_directory(dir.c_str()) == TM_OK);

        // Directories should exist.
        REQUIRE(tmu_directory_exists(dir.c_str()).exists);
        REQUIRE(mock.entries.size() == 4);
    }

    // Delete directories one by one.
    for (int i = (int)size(dirs); i > 0; --i) {
        auto dir = get_dir(base, dirs, i);
        REQUIRE(tmu_delete_directory(dir.c_str()) == TM_OK);
    }
    REQUIRE(tmu_delete_directory(base) == TM_OK);
    REQUIRE(mock.entries.empty());
}

TEST_CASE("fopen") {
    const char* fn1 = "test_file";
    const char* fn2 = "test_file2";

    allocation_guard alloc_guard;
    mock.clear();
    mock.entries.push_back({fn1, nullptr, mock_file});
    mock.entries.push_back({fn2, nullptr, mock_file});

    REQUIRE(tmu_file_exists(fn1).exists);
    REQUIRE(tmu_file_exists(fn2).exists);

    auto f = tmu_fopen(fn1, "rb");
    REQUIRE(f);

    f = tmu_freopen(fn2, "rb", f);
    CHECK(f);

    if (f) fclose(f);
    REQUIRE(!mock.file.is_open);
}

TEST_CASE("unicode") {
    allocation_guard alloc_guard;
    mock.clear();

    mock.entries.push_back({"utf8", (const char*)utf8_bom + 3, std::size(utf8_bom) - 3, mock_file});
    mock.entries.push_back({"utf8_bom", (const char*)utf8_bom, std::size(utf8_bom), mock_file});
    mock.entries.push_back({"utf16le", (const char*)utf16le_bom + 2, std::size(utf16le_bom) - 2, mock_file});
    mock.entries.push_back({"utf16le_bom", (const char*)utf16le_bom, std::size(utf16le_bom), mock_file});
    mock.entries.push_back({"utf16be", (const char*)utf16be_bom + 2, std::size(utf16be_bom) - 2, mock_file});
    mock.entries.push_back({"utf16be_bom", (const char*)utf16be_bom, std::size(utf16be_bom), mock_file});

    struct Pair {
        const char* fn;
        tmu_encoding encoding;
    };

    const Pair pairs[] = {
        {"utf8", tmu_encoding_utf8},       {"utf8_bom", tmu_encoding_utf8_bom},
        {"utf16be", tmu_encoding_utf16be}, {"utf16be_bom", tmu_encoding_utf16be_bom},
        {"utf16le", tmu_encoding_utf16le}, {"utf16le_bom", tmu_encoding_utf16le_bom},
    };

    auto utf8 = tmu_read_file_as_utf8_managed_ex(pairs[0].fn, tmu_encoding_utf8, tmu_validate_error, nullptr);
    REQUIRE(utf8.ec == TM_OK);
    REQUIRE(utf8.contents.size > 0);

    for (auto& pair : pairs) {
        auto other = tmu_read_file_as_utf8_managed_ex(pair.fn, pair.encoding, tmu_validate_error, nullptr);
        CAPTURE(pair.fn);
        REQUIRE(other.ec == TM_OK);
        REQUIRE(other.contents.size > 0);
        REQUIRE(other.contents.size == utf8.contents.size);
        REQUIRE(other.contents.size < other.contents.capacity);  // Nullterminator is not part of size.
        REQUIRE(other.contents.data[other.contents.size] == 0);  // Must be null-terminated.
        REQUIRE(memcmp(other.contents.data, utf8.contents.data, utf8.contents.size) == 0);
    }
}

TEST_CASE("current working dir") {
    allocation_guard alloc_guard;
    mock.clear();

    tm_size_t extra_size = 20;
    auto cwd = tmu_current_working_directory_managed(extra_size);

    REQUIRE(cwd.ec == TM_OK);

    // Success checks.
    REQUIRE(cwd.contents.size > 0);
    REQUIRE(cwd.contents.capacity > 0);
    REQUIRE(cwd.contents.size <= cwd.contents.capacity);
    REQUIRE(cwd.contents.data != nullptr);

    CAPTURE(cwd.contents.data);

    // Directories always have a trailing path delimiter '/'.
    REQUIRE(cwd.contents.data[cwd.contents.size - 1] == '/');

    // Nullterminated.
    REQUIRE(cwd.contents.size < cwd.contents.capacity);
    REQUIRE(cwd.contents.data[cwd.contents.size] == 0);

    // Check whether there is room for extra_size and a nullterminator.
    REQUIRE(cwd.contents.size + extra_size + 1 <= cwd.contents.capacity);
}

#ifndef USE_WINDOWS_H
TEST_CASE("error reporting") {
    const int mock_error = 0x7FFFFFFF;
    const char* fn = "test";
    const char* fn2 = "test2";
    const char* fn3 = "test3";
    const char test_data = 0;

    allocation_guard alloc_guard;
    mock.clear();
    mock.entries.push_back({fn, fn, mock_file});
    mock.entries.push_back({fn3, fn3, mock_file});

    mock.set_fail(fail_stat, mock_error);
    CHECK(tmu_file_exists(fn).ec == mock_error);

    mock.set_fail(fail_stat, mock_error);
    CHECK(tmu_directory_exists(fn).ec == mock_error);

    mock.set_fail(fail_stat, mock_error);
    CHECK(tmu_file_timestamp(fn).ec == mock_error);

    mock.file = {};
    mock.set_fail(fail_fopen, mock_error);
    CHECK(tmu_read_file(fn).ec == mock_error);
    CHECK(!mock.file.is_open);
    mock.file = {};
    mock.set_fail(fail_fread, mock_error);
    CHECK(tmu_read_file(fn).ec == mock_error);
    CHECK(!mock.file.is_open);

    mock.file = {};
    mock.set_fail(fail_fopen, mock_error);
    CHECK(tmu_read_file_as_utf8(fn).ec == mock_error);
    CHECK(!mock.file.is_open);
    mock.file = {};
    mock.set_fail(fail_fread, mock_error);
    CHECK(tmu_read_file_as_utf8(fn).ec == mock_error);
    CHECK(!mock.file.is_open);

    mock.file = {};
    mock.set_fail(fail_fopen, mock_error);
    CHECK(tmu_read_file(fn).ec == mock_error);
    CHECK(!mock.file.is_open);
    mock.file = {};
    mock.set_fail(fail_fread, mock_error);
    CHECK(tmu_read_file(fn).ec == mock_error);
    CHECK(!mock.file.is_open);

    mock.file = {};
    mock.set_fail(fail_mkdir, mock_error);
    CHECK(tmu_write_file_ex("a/b/c/d", &test_data, 1, tmu_create_directory_tree).ec == mock_error);
    CHECK(!mock.file.is_open);
    CHECK(mock.entries.size() == 2);
    mock.file = {};
    mock.set_fail(fail_stat, mock_error);
    CHECK(tmu_write_file_ex(fn, &test_data, 1, 0).ec == mock_error);
    CHECK(!mock.file.is_open);
    CHECK(mock.entries.size() == 2);
    mock.file = {};
    mock.set_fail(fail_fopen, mock_error);
    CHECK(tmu_write_file_ex(fn, &test_data, 1, tmu_overwrite).ec == mock_error);
    CHECK(!mock.file.is_open);
    CHECK(mock.entries.size() == 2);
    mock.file = {};
    mock.set_fail(fail_fwrite, mock_error);
    CHECK(tmu_write_file_ex(fn, &test_data, 1, tmu_overwrite).ec == mock_error);
    CHECK(!mock.file.is_open);
    CHECK(mock.entries.size() == 2);

    mock.file = {};
    mock.set_fail(fail_fopen, mock_error);
    CHECK(tmu_write_file_as_utf8(fn, &test_data, 1).ec == mock_error);
    CHECK(!mock.file.is_open);
    mock.file = {};
    mock.set_fail(fail_fwrite, mock_error);
    CHECK(tmu_write_file_as_utf8(fn, &test_data, 1).ec == mock_error);
    CHECK(!mock.file.is_open);

#if 0
    /* Atomic write not implemented yet. */
    mock.fopen_should_fail_with = mock_error;
    CHECK(tmu_write_file_ex(fn, &test_data, 1, tmu_atomic_write).ec == mock_error);
    mock.fwrite_should_fail_with = mock_error;
    CHECK(tmu_write_file_ex(fn, &test_data, 1, tmu_atomic_write).ec == mock_error);
    mock.fwrite_should_fail_with = mock_error;
    CHECK(tmu_write_file_ex(fn, &test_data, 1, tmu_atomic_write).ec == mock_error);
#endif

    mock.set_fail(fail_stat, mock_error);
    CHECK(tmu_rename_file_ex(fn, fn2, 0) == mock_error);
    CHECK(mock.entries.size() == 2);
    mock.set_fail(fail_rename, mock_error);
    CHECK(tmu_rename_file_ex(fn, fn2, 0) == mock_error);
    CHECK(mock.entries.size() == 2);
    mock.set_fail(fail_rename, mock_error);
    mock.set_fail(fail_stat, mock_error);
    CHECK(tmu_rename_file_ex(fn, fn2, tmu_overwrite) == mock_error);
    CHECK(mock.entries.size() == 2);
    mock.set_fail(fail_rename, mock_error);
    mock.set_fail(fail_remove, mock_error);
    CHECK(tmu_rename_file_ex(fn, fn3, tmu_overwrite) == mock_error);
    CHECK(mock.entries.size() == 2);
    mock.set_fail(fail_mkdir, mock_error);
    CHECK(tmu_rename_file_ex(fn, "a/b/c/d", tmu_create_directory_tree) == mock_error);
    CHECK(mock.entries.size() == 2);

    mock.set_fail(fail_remove, mock_error);
    CHECK(tmu_delete_file(fn) == mock_error);
    CHECK(mock.entries.size() == 2);

    mock.set_fail(fail_mkdir, mock_error);
    CHECK(tmu_create_directory(fn2) == mock_error);
    CHECK(mock.entries.size() == 2);

    mock.set_fail(fail_rmdir, mock_error);
    CHECK(tmu_delete_directory(fn2) == mock_error);
    CHECK(mock.entries.size() == 2);
}
#endif /* !defined(USE_WINDOWS_H) */

#ifdef USE_WINDOWS_H
TEST_CASE("error reporting") {
    const char* fn = "abc/test";
    const char* fn3 = "def/test3";
    char test_data = 0;
    char very_long_fn[1024];
    for (auto i = 0; i < 1024; ++i) very_long_fn[i] = 'X';
    very_long_fn[1023] = 0; /* Nullterminate */
    const char* fn2 = "test/test2";

    allocation_guard alloc_guard;
    mock.clear();
    mock.entries.push_back({fn, fn, mock_file});
    mock.entries.push_back({fn3, fn3, mock_file});

    auto do_test = [](should_fail_enum which, const char* func_string, auto test_func) {
        CAPTURE(func_string);
        allocation_guard alloc_guard;
        mock.file = {};
        mock.set_fail(which, ERROR_MOCK);
        auto result = test_func();
        CHECK(result);
        CHECK(mock.entries.size() == 2);
        CHECK(!mock.file.is_open);
        CHECK(mock.readdir_index == 0);
    };

#define check(which, cond) do_test((which), #cond, [&]() { return (cond) != TM_OK; })

    check(fail_GetFileAttributesW, tmu_file_exists(fn).ec);
    check(fail_MultiByteToWideChar, tmu_file_exists(fn).ec);

    check(fail_GetFileAttributesW, tmu_directory_exists(fn).ec);
    check(fail_MultiByteToWideChar, tmu_directory_exists(fn).ec);

    check(fail_GetFileAttributesExW, tmu_file_timestamp(fn).ec);
    check(fail_MultiByteToWideChar, tmu_file_timestamp(fn).ec);

    check(fail_MultiByteToWideChar, tmu_read_file(fn).ec);
    check(fail_CreateFileW, tmu_read_file(fn).ec);
    check(fail_GetFileSizeEx, tmu_read_file(fn).ec);
    check(fail_ReadFile, tmu_read_file(fn).ec);

    check(fail_MultiByteToWideChar, tmu_write_file_ex(fn, &test_data, sizeof(char), 0).ec);
    check(fail_CreateFileW, tmu_write_file_ex(fn, &test_data, sizeof(char), 0).ec);
    check(fail_CreateFileW, tmu_write_file_ex(fn, &test_data, sizeof(char), tmu_overwrite).ec);
    check(fail_WriteFile, tmu_write_file_ex(fn, &test_data, sizeof(char), 0).ec);
    check(fail_WriteFile, tmu_write_file_ex(fn, &test_data, sizeof(char), tmu_overwrite).ec);
    check(fail_CreateDirectoryW, tmu_write_file_ex(fn, &test_data, sizeof(char), tmu_create_directory_tree).ec);

    /* Atomic Write */
    check(fail_MultiByteToWideChar, tmu_write_file_ex(fn, &test_data, sizeof(char), tmu_atomic_write).ec);
    check(fail_GetTempFileNameW, tmu_write_file_ex(fn, &test_data, sizeof(char), tmu_atomic_write).ec);
    check(fail_CreateFileW, tmu_write_file_ex(fn, &test_data, sizeof(char), 0).ec);
    check(fail_WriteFile, tmu_write_file_ex(fn, &test_data, sizeof(char), 0).ec);
    check(fail_CreateFileW, tmu_write_file_ex(very_long_fn, &test_data, sizeof(char), tmu_atomic_write).ec);

    check(fail_MultiByteToWideChar, tmu_write_file_as_utf8(fn, &test_data, sizeof(char)).ec);
    check(fail_CreateFileW, tmu_write_file_as_utf8(fn, &test_data, sizeof(char)).ec);
    check(fail_WriteFile, tmu_write_file_as_utf8(fn, &test_data, sizeof(char)).ec);
    check(fail_CreateDirectoryW, tmu_write_file_as_utf8_ex(fn, &test_data, sizeof(char), tmu_create_directory_tree).ec);

    check(fail_MultiByteToWideChar, tmu_rename_file_ex(fn, fn2, 0));
    check(fail_MoveFileExW, tmu_rename_file_ex(fn, fn2, 0));
    check(fail_MoveFileExW, tmu_rename_file_ex(fn, fn2, tmu_overwrite));
    check(fail_none, tmu_rename_file_ex(fn, fn3, 0));
    check(fail_CreateDirectoryW, tmu_rename_file_ex(fn, fn2, tmu_create_directory_tree));

    check(fail_MultiByteToWideChar, tmu_delete_file(fn));
    check(fail_DeleteFileW, tmu_delete_file(fn));

    check(fail_MultiByteToWideChar, tmu_delete_directory(fn2));
    check(fail_RemoveDirectoryW, tmu_delete_directory(fn2));

    check(fail_GetCurrentDirectoryW, tmu_current_working_directory(0).ec);
    check(fail_WideCharToMultiByte, tmu_current_working_directory(0).ec);

    check(fail_malloc, tmu_open_directory(".").ec);
    check(fail_FindFirstFileW, tmu_open_directory(".").ec);
#undef check
}

TEST_CASE("winapi command line") {
    allocation_guard alloc_guard;

    auto result = tmu_utf8_winapi_get_command_line_managed();
    REQUIRE(result.ec == TM_OK);
    REQUIRE(result.command_line.args);
    REQUIRE(result.command_line.args_count == 1);
    REQUIRE(*result.command_line.args);
    REQUIRE(result.command_line.args[0]);
    REQUIRE(*result.command_line.args[0]);
    REQUIRE(result.command_line.args[1]);
    REQUIRE(*result.command_line.args[1] == 0);
    REQUIRE(result.command_line.internal_buffer);

    mock.set_fail(fail_GetCommandLineW, ERROR_MOCK);
    result = tmu_utf8_winapi_get_command_line_managed();
    REQUIRE(result.ec != TM_OK);

    mock.set_fail(fail_GetCommandLineW, ERROR_MOCK_INVALID_COMMAND_LINE);
    result = tmu_utf8_winapi_get_command_line_managed();
    REQUIRE(result.ec != TM_OK);

    mock.set_fail(fail_CommandLineToArgvW, ERROR_MOCK);
    result = tmu_utf8_winapi_get_command_line_managed();
    REQUIRE(result.ec != TM_OK);
}

#endif /* defined(USE_WINDOWS_H) */

TEST_CASE("command line") {
    allocation_guard alloc_guard;

    struct static_wide_string {
        uint16_t entries[100];
    };

    auto make_wide = [](std::string_view ascii) {
        static_wide_string result;
        TM_ASSERT(ascii.size() < std::size(result.entries));

        const char* in = ascii.data();
        uint16_t* out = result.entries;

        for (size_t i = 0, count = ascii.size(); i < count; ++i) {
            *out++ = (uint16_t)(*in++);
        }
        *out = 0;
        return result;
    };

    const int entries_count = 5;
    std::string_view ascii_options[entries_count] = {
        "test.exe",
        "-n=some option",
        "some more strings",
        "etc",
        "",
    };

    static_wide_string command_line_pool[entries_count] = {
        make_wide(ascii_options[0]),
        make_wide(ascii_options[1]),
        make_wide(ascii_options[2]),
        make_wide(ascii_options[3]),
        make_wide(ascii_options[4]),
    };

    uint16_t* const wide_args[entries_count] = {
        command_line_pool[0].entries, command_line_pool[1].entries, command_line_pool[2].entries,
        command_line_pool[3].entries, command_line_pool[4].entries,
    };

    {
        auto result = tmu_utf8_command_line_from_utf16_managed(wide_args, entries_count - 1);
        REQUIRE(result.ec == TM_OK);
        REQUIRE(result.command_line.args);
        REQUIRE(result.command_line.internal_buffer);
        REQUIRE(result.command_line.args_count == entries_count - 1);

        for (int i = 0; i < entries_count; i++) {
            REQUIRE(result.command_line.args[i] == ascii_options[i]);
        }
    }

    {
        auto result = tmu_utf8_command_line_from_utf16_managed(wide_args, 0);
        REQUIRE(result.ec == TM_OK);
        REQUIRE(result.command_line.args);
        REQUIRE(result.command_line.internal_buffer);
        REQUIRE(result.command_line.args[0]);
        REQUIRE(*result.command_line.args[0] == 0);
    }

    {
        const uint16_t invalid[2] = {0xDC00u, 0};
        uint16_t empty = 0;

        uint16_t const * const invalid_args[2] = {invalid, &empty};
        auto result = tmu_utf8_command_line_from_utf16_managed(invalid_args, 1);
        REQUIRE(result.ec == TM_EINVAL);
    }
}

TEST_CASE("positive directory reading") {
    allocation_guard alloc_guard;

    mock.entries.clear();
    mock.entries.push_back({".", "", mock_dir});
    mock.entries.push_back({"..", "", mock_dir});
    mock.entries.push_back({"a", "", mock_dir});
    mock.entries.push_back({"b", "", mock_dir});
    mock.entries.push_back({"c", "", mock_file});
    mock.entries.push_back({"d", "", mock_file});
    mock.entries.push_back({"e", "", mock_dir});

    auto dir = tmu_open_directory("./");
    REQUIRE(dir.ec == TM_OK);
    auto entry = tmu_read_directory(&dir);
    REQUIRE(dir.ec == TM_OK);
    REQUIRE(entry);
    REQUIRE(std::string_view{entry->name} == "a");
    REQUIRE(entry->is_file == false);
    entry = tmu_read_directory(&dir);
    REQUIRE(dir.ec == TM_OK);
    REQUIRE(entry);
    REQUIRE(std::string_view{entry->name} == "b");
    REQUIRE(entry->is_file == false);
    entry = tmu_read_directory(&dir);
    REQUIRE(dir.ec == TM_OK);
    REQUIRE(entry);
    REQUIRE(std::string_view{entry->name} == "c");
    REQUIRE(entry->is_file == true);
    entry = tmu_read_directory(&dir);
    REQUIRE(dir.ec == TM_OK);
    REQUIRE(entry);
    REQUIRE(std::string_view{entry->name} == "d");
    REQUIRE(entry->is_file == true);
    entry = tmu_read_directory(&dir);
    REQUIRE(dir.ec == TM_OK);
    REQUIRE(entry);
    REQUIRE(std::string_view{entry->name} == "e");
    REQUIRE(entry->is_file == false);
    entry = tmu_read_directory(&dir);
    REQUIRE(dir.ec == TM_OK);
    REQUIRE(!entry);
    tmu_close_directory(&dir);
}

TEST_CASE("module filename") {
    allocation_guard alloc_guard;

    std::string_view expected = "some/dir/and/a/file.exe";
    std::string_view expected_dir = "some/dir/and/a/";
    mock.set_module_filename(expected);

    auto res = tmu_module_filename();
    REQUIRE(res.ec == TM_OK);
    REQUIRE(res.contents.data == expected);
    tmu_destroy_contents(&res.contents);

    res = tmu_module_directory();
    REQUIRE(res.ec == TM_OK);
    REQUIRE(res.contents.data == expected_dir);
    tmu_destroy_contents(&res.contents);

    char long_name[2048];
    for (int i = 0; i < 2047; ++i) {
        long_name[i] = 'X';
    }
    long_name[2047] = 0;
    expected = long_name;
    expected_dir = "";
    mock.set_module_filename(expected);

    res = tmu_module_filename();
    REQUIRE(res.ec == TM_OK);
    REQUIRE(res.contents.data == expected);
    tmu_destroy_contents(&res.contents);

    res = tmu_module_directory();
    REQUIRE(res.ec == TM_OK);
    REQUIRE(res.contents.data == expected_dir);
    tmu_destroy_contents(&res.contents);
}

TEST_CASE("console output") {
    allocation_guard overall_alloc_guard;

// Preprocessor defined, so that we do not run into Wformat-security when the format string isn't a string literal.
#define long_str                                                                       \
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \

    {
        allocation_guard alloc_guard;
        tmu_console_output_init();
        const char* test = "test_output";
        tmu_console_output(tmu_console_out, test);
        REQUIRE(mock.out.has_output(test));
        mock.clear();
    }

    {
        allocation_guard alloc_guard;
        tmu_console_output_init();
        std::string_view test = "test_output";
        tmu_console_output_n(tmu_console_out, test.data(), (tm_size_t)test.size());
        REQUIRE(mock.out.has_output(test));
        mock.clear();
    }

    {
        allocation_guard alloc_guard;
        tmu_console_output_init();
        std::string_view test = long_str;
        tmu_console_output_n(tmu_console_out, test.data(), (tm_size_t)test.size());
        REQUIRE(mock.out.has_output(test));
        mock.clear();
    }

#define test_format "This is a test format %d"
    {
        allocation_guard alloc_guard;
        tmu_console_output_init();
        int result = tmu_printf(test_format, 1);
        std::string_view format = test_format;
        REQUIRE(result == (int)format.size());
        REQUIRE(mock.out.has_output(format));
        mock.clear();
    }

    {
        allocation_guard alloc_guard;
        tmu_console_output_init();
        int result = tmu_printf(long_str);
        std::string_view format = long_str;
        REQUIRE(result == (int)format.size());
        REQUIRE(mock.out.has_output(format));
        mock.clear();
    }

    {
        allocation_guard alloc_guard;
        tmu_console_output_init();
        int result = tmu_fprintf(&mock.out.file, test_format, 1);
        std::string_view format = test_format;
        REQUIRE(result == (int)format.size());
        REQUIRE(mock.out.has_output(test_format));
        mock.clear();
    }

    {
        allocation_guard alloc_guard;
        mock.out.is_file = true;
        tmu_console_output_init();
        int result = tmu_fprintf(&mock.out.file, test_format, 1);
        std::string_view format = test_format;
        REQUIRE(result == (int)format.size());
        REQUIRE(mock.out.has_output(test_format));
        mock.clear();
    }

    {
        allocation_guard alloc_guard;
        tmu_console_output_init();
        int result = tmu_fprintf(&mock.out.file, long_str);
        std::string_view format = long_str;
        REQUIRE(result == (int)format.size());
        REQUIRE(mock.out.has_output(format));
        mock.clear();
    }

    {
        allocation_guard alloc_guard;
        mock.out.is_file = true;
        tmu_console_output_init();
        int result = tmu_fprintf(&mock.out.file, long_str);
        std::string_view format = long_str;
        REQUIRE(result == (int)format.size());
        REQUIRE(mock.out.has_output(format));
        mock.clear();
    }
}