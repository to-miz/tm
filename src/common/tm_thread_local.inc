#ifndef TM_THREAD_LOCAL
    #ifdef __cplusplus
        #define TM_THREAD_LOCAL thread_local
    #elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
        #define TM_THREAD_LOCAL _Thread_local
    #elif defined(__GNUC__) || defined(__clang__) || defined(__MINGW32__) || defined(__SUNPRO_C) || defined(__xlC__)
        #define TM_THREAD_LOCAL __thread
    #elif defined(_WIN32) && (defined(_MSC_VER) || defined(__ICL) || defined(__DMC__) || defined(__BORLANDC__))
        #define TM_THREAD_LOCAL __declspec(thread)
    #else
        #error "Unsupported platform for TM_THREAD_LOCAL"
    #endif
#endif