/* Common POSIX compatible error codes. You can override the definitions by defining TM_ERRC_DEFINED
   before including this file. */
#ifndef TM_ERRC_DEFINED
    #define TM_ERRC_DEFINED
    enum TM_ERRC_CODES {
        TM_OK           = 0,   /* same as std::errc() */
        TM_ENOENT       = 2,   /* same as std::errc::no_such_file_or_directory */
        TM_EIO          = 5,   /* same as std::errc::io_error */
        TM_ENOMEM       = 12,  /* same as std::errc::not_enough_memory */
        TM_EACCES       = 13,  /* same as std::errc::permission_denied */
        TM_EBUSY        = 16,  /* same as std::errc::device_or_resource_busy */
        TM_EEXIST       = 17,  /* same as std::errc::file_exists */
        TM_ENODEV       = 19,  /* same as std::errc::no_such_device */
        TM_EINVAL       = 22,  /* same as std::errc::invalid_argument */
        TM_EFBIG        = 27,  /* same as std::errc::file_too_large */
        TM_ENOSPC       = 28,  /* same as std::errc::no_space_on_device */
        TM_ERANGE       = 34,  /* same as std::errc::result_out_of_range */
        TM_ENAMETOOLONG = 36,  /* same as std::errc::filename_too_long */
        TM_ENOTEMPTY    = 39,  /* same as std::errc::directory_not_empty */
        TM_EOVERFLOW    = 75,  /* same as std::errc::value_too_large */
    };
    typedef int tm_errc;
#endif