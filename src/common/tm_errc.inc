/* Common POSIX compatible error codes. You can override the definitions by defining TM_ERRC_DEFINED
   before including this file. */
#ifndef TM_ERRC_DEFINED
    #define TM_ERRC_DEFINED
    enum TM_ERRC_CODES {
        TM_OK           = 0,   /* Same as std::errc() */
        TM_EPERM        = 1,   /* Same as std::errc::operation_not_permitted */
        TM_ENOENT       = 2,   /* Same as std::errc::no_such_file_or_directory */
        TM_EIO          = 5,   /* Same as std::errc::io_error */
        TM_ENOMEM       = 12,  /* Same as std::errc::not_enough_memory */
        TM_EACCES       = 13,  /* Same as std::errc::permission_denied */
        TM_EBUSY        = 16,  /* Same as std::errc::device_or_resource_busy */
        TM_EEXIST       = 17,  /* Same as std::errc::file_exists */
        TM_EEXDEV       = 18,  /* Same as std::errc::cross_device_link */
        TM_ENODEV       = 19,  /* Same as std::errc::no_such_device */
        TM_EINVAL       = 22,  /* Same as std::errc::invalid_argument */
        TM_EMFILE       = 24,  /* Same as std::errc::too_many_files_open */
        TM_EFBIG        = 27,  /* Same as std::errc::file_too_large */
        TM_ENOSPC       = 28,  /* Same as std::errc::no_space_on_device */
        TM_ERANGE       = 34,  /* Same as std::errc::result_out_of_range */
        TM_ENAMETOOLONG = 36,  /* Same as std::errc::filename_too_long */
        TM_ENOTEMPTY    = 39,  /* Same as std::errc::directory_not_empty */
        TM_EOVERFLOW    = 75,  /* Same as std::errc::value_too_large */
    };
    typedef int tm_errc;
#endif