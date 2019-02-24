/* C++ string_view support. If TM_STRING_VIEW is defined, so must be TM_STRING_VIEW_DATA, TM_STRING_VIEW_SIZE
   and TM_STRING_VIEW_MAKE.
   Example:
        #include <string_view>
        #define TM_STRING_VIEW std::string_view
        #define TM_STRING_VIEW_DATA(str) (str).data()
        #define TM_STRING_VIEW_SIZE(str) ((tm_size_t)(str).size())
        #define TM_STRING_VIEW_MAKE(data, size) std::string_view{(data), (size_t)(size)}
*/
#ifdef TM_STRING_VIEW
    #if !defined(TM_STRING_VIEW_DATA) || !defined(TM_STRING_VIEW_SIZE) || !defined(TM_STRING_VIEW_MAKE)
        #error Invalid TM_STRINV_VIEW. If TM_STRING_VIEW is defined, so must be TM_STRING_VIEW_DATA, \
TM_STRING_VIEW_SIZE and TM_STRING_VIEW_MAKE.
    #endif
#endif