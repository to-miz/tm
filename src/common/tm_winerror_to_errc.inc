static tm_errc MERGE_WINERROR_TO_ERRC(DWORD error, tm_errc def) {
    switch (error) {
        case ERROR_ACCESS_DENIED:
        case ERROR_CANNOT_MAKE:
        case ERROR_CURRENT_DIRECTORY:
        case ERROR_INVALID_ACCESS:
        case ERROR_NOACCESS:
        case ERROR_SHARING_VIOLATION:
        case ERROR_WRITE_PROTECT: {
            return TM_EACCES;
        }
        case ERROR_ALREADY_EXISTS:
        case ERROR_FILE_EXISTS: {
            return TM_EEXIST;
        }
        case ERROR_CANTOPEN:
        case ERROR_CANTREAD:
        case ERROR_CANTWRITE:
        case ERROR_OPEN_FAILED:
        case ERROR_READ_FAULT:
        case ERROR_SEEK:
        case ERROR_WRITE_FAULT: {
            return TM_EIO;
        }
        case ERROR_DIRECTORY:
        case ERROR_INVALID_HANDLE:
        case ERROR_INVALID_NAME:
        case ERROR_NEGATIVE_SEEK:
        case ERROR_NO_UNICODE_TRANSLATION:
        case ERROR_INVALID_PARAMETER:
        case ERROR_INVALID_FLAGS: {
            return TM_EINVAL;
        }
        case ERROR_INSUFFICIENT_BUFFER: {
            return TM_ERANGE;
        }
        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND: {
            return TM_ENOENT;
        }
        case ERROR_NOT_ENOUGH_MEMORY:
        case ERROR_OUTOFMEMORY: {
            return TM_ENOMEM;
        }
        case ERROR_BAD_UNIT:
        case ERROR_DEV_NOT_EXIST:
        case ERROR_INVALID_DRIVE: {
            return TM_ENODEV;
        }
        case ERROR_BUSY:
        case ERROR_BUSY_DRIVE:
        case ERROR_DEVICE_IN_USE:
        case ERROR_OPEN_FILES: {
            return TM_EBUSY;
        }
        case ERROR_DISK_FULL:
        case ERROR_HANDLE_DISK_FULL: {
            return TM_ENOSPC;
        }
        case ERROR_BUFFER_OVERFLOW: {
            return TM_ENAMETOOLONG;
        }
        case ERROR_DIR_NOT_EMPTY: {
            return TM_ENOTEMPTY;
        }
        case ERROR_NOT_SAME_DEVICE: {
            return TM_EXDEV;
        }
        case ERROR_TOO_MANY_OPEN_FILES: {
            return TM_EMFILE;
        }
        case ERROR_NOT_READY:
        case ERROR_RETRY: {
            return TM_EAGAIN;
        }
        case ERROR_INVALID_FUNCTION: {
            return TM_ENOSYS;
        }
        case ERROR_LOCK_VIOLATION:
        case ERROR_LOCKED: {
            return TM_ENOLCK;
        }
        case ERROR_OPERATION_ABORTED: {
            return TM_ECANCELED;
        }
        default: {
            return def;
        }
    }
}
#undef MERGE_WINERROR_TO_ERRC