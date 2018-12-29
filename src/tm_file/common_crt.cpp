
tmf_exists_result tmf_file_exists_t(const tmf_tchar* filename) {
    TM_ASSERT(filename);

    tmf_exists_result result = {TM_FALSE, TM_OK};

    TMF_STRUCT_STAT buffer;
    int stat_result = TMF_STAT(filename, &buffer);

    if (stat_result == 0) {
        result.exists = (buffer.st_mode & TMF_S_IFREG) != 0;
    } else if (stat_result == -1) {
        result.exists = false;
    } else {
        result.ec = TM_EINVAL;
    }

    return result;
}

tmf_exists_result tmf_directory_exists_t(const tmf_tchar* dir) {
    TM_ASSERT(dir);

    tmf_exists_result result = {TM_FALSE, TM_OK};

    TMF_STRUCT_STAT buffer;
    int stat_result = TMF_STAT(dir, &buffer);

    if (stat_result == 0) {
        result.exists = (buffer.st_mode & TMF_S_IFDIR) != 0;
    } else if (stat_result == -1) {
        result.exists = false;
    } else {
        result.ec = TM_EINVAL;
    }

    return result;
}

tmf_file_timestamp_result tmf_file_timestamp_t(const tmf_tchar* dir) {
    TM_ASSERT(dir);

    tmf_file_timestamp_result result = {0, TM_OK};

    TMF_STRUCT_STAT buffer;
    int stat_result = TMF_STAT(dir, &buffer);

    if (stat_result == 0) {
        result.file_time = buffer.st_mtime;
    } else {
        result.ec = (stat_result == TM_EINVAL) ? TM_EINVAL : TM_ENOENT;
    }

    return result;
}

static tm_errc tmf_create_single_directory_t(const tmf_tchar* dir) {
    tm_errc result = TM_OK;

    int err = TMF_MKDIR(dir);
    if (err != 0) {
        if (errno != EEXIST) {
            result = errno;
        }
    }
    return result;
}

tm_errc tmf_delete_file_t(const tmf_tchar* filename) {
    tm_errc result = TM_OK;
    if (TMF_REMOVE(filename) != 0) result = errno;
    return result;
}

tm_errc tmf_delete_directory_t(const tmf_tchar* dir) {
    tm_errc result = TM_OK;
    if (TMF_RMDIR(dir) != 0) result = errno;
    return result;
}

tm_errc tmf_rename_file_ex_t(const tmf_tchar* from, const tmf_tchar* to, uint32_t flags) {
    tm_errc result = TM_OK;

    int err = TMF_RENAME(from, to);
    if (err != 0) {
        if (errno == EACCES && (flags & tmf_overwrite)) {
            /* Destination probably exists, try to delete file since tmf_overwrite is specified and try again. */
            result = tmf_delete_file_t(to);
            if (result == TM_OK) {
                err = TMF_RENAME(from, to);
                if (err != 0) result = errno;
            }
        } else {
            result = errno;
        }
    }
    return result;
}

#undef TMF_STAT
#undef TMF_STRUCT_STAT
#undef TMF_S_IFDIR
#undef TMF_S_IFREG
#undef TMF_MKDIR
#undef TMF_RMDIR
#undef TMF_REMOVE
#undef TMF_RENAME
#undef TMF_GETCWD