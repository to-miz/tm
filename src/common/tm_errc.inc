/* Common POSIX compatible error codes. You can override the definitions by defining TM_ERRC_DEFINED
   before including this file. */
#ifndef TM_ERRC_DEFINED
    #define TM_ERRC_DEFINED
    enum TM_ERRC_CODES {
        TM_OK        = 0,   /* same as std::errc() */
        TM_EOVERFLOW = 75,  /* same as std::errc::value_too_large */
        TM_ERANGE    = 34,  /* same as std::errc::result_out_of_range */
        TM_EINVAL    = 22,  /* same as std::errc::invalid_argument */
        TM_ENOMEM    = 12,  /* same as std::errc::not_enough_memory */
    };
    typedef int tm_errc;
#endif