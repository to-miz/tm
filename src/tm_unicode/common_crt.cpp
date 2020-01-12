static tmu_exists_result tmu_file_exists_t(const tmu_tchar* filename) {
    TM_ASSERT(filename);

    tmu_exists_result result = {TM_FALSE, TM_OK};

    TMU_STRUCT_STAT buffer;
    int stat_result = TMU_STAT(filename, &buffer);

    if (stat_result == 0) {
        result.exists = TMU_S_ISREG(buffer.st_mode);
    } else if (stat_result == -1) {
        /* TODO: Which version to use?
           One is picky about the error to report, the other is picky about saying anything about the existence.
           It depends on the implementation details of stat (Unix vs Windows report different errors etc.). */
#if 1
        if (errno == ENOENT || errno == ENOTDIR) {
            result.exists = TM_FALSE;
        } else {
            result.ec = errno;
        }
#else
        if (errno == EINVAL || errno == EACCES || errno == EOVERFLOW) {
            result.ec = errno;
        } else {
            result.exists = TM_FALSE;
        }
#endif
    } else {
        /* On Windows stat doesn't return -1 on EINVAL. */
        result.ec = TM_EINVAL;
    }

    return result;
}

static tmu_exists_result tmu_directory_exists_t(const tmu_tchar* dir) {
    TM_ASSERT(dir);

    tmu_exists_result result = {TM_FALSE, TM_OK};

    TMU_STRUCT_STAT buffer;
    int stat_result = TMU_STAT(dir, &buffer);

    if (stat_result == 0) {
        result.exists = TMU_S_ISDIR(buffer.st_mode);
    } else if (stat_result == -1) {
        /* TODO: Which version to use?
           One is picky about the error to report, the other is picky about saying anything about the existence.
           It depends on the implementation details of stat (Unix vs Windows report different errors etc.). */
#if 1
        if (errno == ENOENT || errno == ENOTDIR) {
            result.exists = TM_FALSE;
        } else {
            result.ec = errno;
        }
#else
        if (errno == EINVAL || errno == EACCES || errno == EOVERFLOW) {
            result.ec = errno;
        } else {
            result.exists = TM_FALSE;
        }
#endif
    } else {
        /* On Windows stat doesn't return -1 on EINVAL. */
        result.ec = TM_EINVAL;
    }

    return result;
}

static tmu_contents_result tmu_read_file_t(const tmu_tchar* filename) {
    tmu_contents_result result = {{TM_NULL, 0, 0}, TM_OK};

    errno = 0;
    FILE* f = tmu_fopen_t(filename, TMU_TEXT("rb"));
    if (!f) {
        result.ec = (errno != 0) ? errno : TM_EIO;
    } else {
        enum { BUFFER_SIZE = 1024 };
        char buffer[BUFFER_SIZE];
        size_t size = fread(buffer, sizeof(char), sizeof(buffer), f);
        if (size) {
            result.contents.capacity = ((tm_size_t)size < BUFFER_SIZE) ? ((tm_size_t)size) : (BUFFER_SIZE * 2);
            result.contents.data = (char*)TMU_MALLOC(result.contents.capacity * sizeof(char), sizeof(char));
            if (!result.contents.data) {
                result.ec = TM_ENOMEM;
            } else {
                TMU_MEMCPY(result.contents.data, buffer, size * sizeof(char));
                result.contents.size = (tm_size_t)size;
                while ((size = fread(buffer, sizeof(char), sizeof(buffer), f)) != 0) {
                    if (!tmu_grow_by(&result.contents, (tm_size_t)size)) {
                        tmu_destroy_contents(&result.contents);
                        result.ec = TM_ENOMEM;
                        break;
                    }
                    TMU_MEMCPY(result.contents.data + result.contents.size, buffer, size * sizeof(char));
                    result.contents.size += (tm_size_t)size;
                }
            }
        }
        if (errno != 0 || ferror(f)) {
            result.ec = (errno != 0) ? errno : TM_EIO;
        }
        fclose(f);
    }

    return result;
}

static tmu_write_file_result tmu_write_file_ex_internal(const tmu_tchar* filename, const void* data, tm_size_t size,
                                                        uint32_t flags) {
    TM_ASSERT_VALID_SIZE(size);
    tmu_write_file_result result = {0, TM_OK};

    if (flags & tmu_create_directory_tree) {
        tm_errc ec = tmu_create_directory_internal(filename, tmu_get_path_len_internal(filename, /*filename_len=*/0));
        if (ec != TM_OK) {
            result.ec = ec;
            return result;
        }
    }

    if (!(flags & tmu_overwrite)) {
        tmu_exists_result exists = tmu_file_exists_t(filename);
        if (exists.ec != TM_OK) {
            result.ec = exists.ec;
            return result;
        }
        if (exists.exists) {
            result.ec = TM_EEXIST;
            return result;
        }
    }

    FILE* f = tmu_fopen_t(filename, TMU_TEXT("wb"));
    if (!f) {
        result.ec = (errno != 0) ? errno : TM_EIO;
        return result;
    }

    errno = 0;
    result.written = (tm_size_t)fwrite(data, 1, size, f);
    if (result.written != size || errno != 0 || ferror(f)) {
        result.ec = (errno != 0) ? errno : TM_EIO;
    }

    fclose(f);
    return result;
}

static tm_errc tmu_delete_file_t(const tmu_tchar* filename) {
    tm_errc result = TM_OK;
    if (TMU_REMOVE(filename) != 0) result = (errno != 0) ? errno : TM_EIO;
    return result;
}

static tm_errc tmu_rename_file_ex_t(const tmu_tchar* from, const tmu_tchar* to, uint32_t flags) {
    tm_errc result = TM_OK;

    /* We check if 'to' exists first, because some implementations of rename overwrite by default. */
    if (!(flags & tmu_overwrite)) {
        tmu_exists_result to_exists = tmu_file_exists_t(to);
        if (to_exists.ec != TM_OK) return to_exists.ec;
        if (to_exists.exists) return TM_EEXIST;
    }

    if (flags & tmu_create_directory_tree) {
        tm_errc ec = tmu_create_directory_internal(to, tmu_get_path_len_internal(to, /*filename_len=*/0));
        if (ec != TM_OK) return ec;
    }

    errno = 0;
    int err = TMU_RENAME(from, to);
    if (err != 0) {
        result = (errno != 0) ? errno : TM_EIO;
        if ((flags & tmu_overwrite) && tmu_file_exists_t(from).exists && tmu_file_exists_t(to).exists) {
            /* Destination probably exists, try to delete file since tmu_overwrite is specified and try again. */
            result = tmu_delete_file_t(to);
            if (result == TM_OK) {
                err = TMU_RENAME(from, to);
                if (err != 0) result = (errno != 0) ? errno : TM_EIO;
            }
        }
    }
    return result;
}

static tmu_write_file_result tmu_write_file_ex_t(const tmu_tchar* filename, const void* data, tm_size_t size,
                                                 uint32_t flags) {
    if (!(flags & tmu_atomic_write)) {
        return tmu_write_file_ex_internal(filename, data, size, flags);
    }

    tmu_write_file_result result = {0, TM_EIO};

#if 0
    /* FIXME: Not implemented. */
    /* Get a temp filename*/
    char temp_filename_buffer[L_tmpnam + 1];
    char* temp_filename = TM_NULL;
    for (int attempts = 0; attempts < 10; ++attempts) {
        // Try to open up a temporary file. If filename was not unique or another process got it, try again.
        temp_filename = tmpnam(temp_filename_buffer);
        if (!temp_filename) {
            result.ec = TM_EEXIST;
            break;
        }
        result = tmu_write_file_ex_internal(temp_filename, data, size, 0);
        if (result.ec == TM_EEXIST) {
            continue;
        } else {
            break;
        }
    }
    if (result.ec == TM_OK) {
        TM_ASSERT(temp_filename);
        result.ec = tmu_rename_file_ex_t(temp_filename, filename, flags);
    }
#endif
    return result;
}

static tmu_file_timestamp_result tmu_file_timestamp_t(const tmu_tchar* dir) {
    TM_ASSERT(dir);

    tmu_file_timestamp_result result = {0, TM_OK};

    TMU_STRUCT_STAT buffer;
    int stat_result = TMU_STAT(dir, &buffer);

    if (stat_result == 0) {
        result.file_time = (tmu_file_time)buffer.st_mtime;
    } else {
        if (errno != 0) {
            result.ec = errno;
        } else {
            result.ec = (stat_result == TM_EINVAL) ? TM_EINVAL : TM_ENOENT;
        }
    }

    return result;
}

static tm_errc tmu_create_single_directory_t(const tmu_tchar* dir) {
    tm_errc result = TM_OK;

    int err = TMU_MKDIR(dir);
    if (err != 0) {
        if (errno != EEXIST) {
            result = errno;
        }
    }
    return result;
}

static tm_errc tmu_delete_directory_t(const tmu_tchar* dir) {
    tm_errc result = TM_OK;
    if (TMU_RMDIR(dir) != 0) result = errno;
    return result;
}

#undef TMU_STAT
#undef TMU_STRUCT_STAT
#undef TMU_S_IFDIR
#undef TMU_S_IFREG
#undef TMU_MKDIR
#undef TMU_RMDIR
#undef TMU_REMOVE
#undef TMU_RENAME
#undef TMU_GETCWD
#undef TMU_FOPEN_READ
#undef TMU_FOPEN_WRITE
