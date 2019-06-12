/* Common POSIX compatible error codes. You can override the definitions by defining TM_ERRC_DEFINED
   before including this file. */
#ifndef TM_ERRC_DEFINED
    #define TM_ERRC_DEFINED
    #include <errno.h>
    enum TM_ERRC_CODES {
        TM_OK           = 0,            /* Alternatively std::errc() */
        TM_EPERM        = EPERM,        /* Alternatively std::errc::operation_not_permitted */
        TM_ENOENT       = ENOENT,       /* Alternatively std::errc::no_such_file_or_directory */
        TM_EIO          = EIO,          /* Alternatively std::errc::io_error */
        TM_EAGAIN       = EAGAIN,       /* Alternatively std::errc::resource_unavailable_try_again */
        TM_ENOMEM       = ENOMEM,       /* Alternatively std::errc::not_enough_memory */
        TM_EACCES       = EACCES,       /* Alternatively std::errc::permission_denied */
        TM_EBUSY        = EBUSY,        /* Alternatively std::errc::device_or_resource_busy */
        TM_EEXIST       = EEXIST,       /* Alternatively std::errc::file_exists */
        TM_EXDEV        = EXDEV,        /* Alternatively std::errc::cross_device_link */
        TM_ENODEV       = ENODEV,       /* Alternatively std::errc::no_such_device */
        TM_EINVAL       = EINVAL,       /* Alternatively std::errc::invalid_argument */
        TM_EMFILE       = EMFILE,       /* Alternatively std::errc::too_many_files_open */
        TM_EFBIG        = EFBIG,        /* Alternatively std::errc::file_too_large */
        TM_ENOSPC       = ENOSPC,       /* Alternatively std::errc::no_space_on_device */
        TM_ERANGE       = ERANGE,       /* Alternatively std::errc::result_out_of_range */
        TM_ENAMETOOLONG = ENAMETOOLONG, /* Alternatively std::errc::filename_too_long */
        TM_ENOLCK       = ENOLCK,       /* Alternatively std::errc::no_lock_available */
        TM_ECANCELED    = ECANCELED,    /* Alternatively std::errc::operation_canceled */
        TM_ENOSYS       = ENOSYS,       /* Alternatively std::errc::function_not_supported */
        TM_ENOTEMPTY    = ENOTEMPTY,    /* Alternatively std::errc::directory_not_empty */
        TM_EOVERFLOW    = EOVERFLOW,    /* Alternatively std::errc::value_too_large */
        TM_ETIMEDOUT    = ETIMEDOUT,    /* Alternatively std::errc::timed_out */
    };
    typedef int tm_errc;
#endif