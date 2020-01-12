template <class... Types>
struct tmp_type_flags;

template <>
struct tmp_type_flags<> {
    enum : uint64_t { value = 0 };
};

template <class... Types>
struct tmp_type_flags<char, Types...> {
    enum : uint64_t { value = PrintType::Char | (tmp_type_flags<Types...>::value << PrintType::Bits) };
};

// integer types to enum mapping based on their size
template <size_t N>
struct tmp_int_size;

template <>
struct tmp_int_size<1> {
    enum : uint64_t { value = PrintType::Int32 };
    typedef int32_t Type;
    static PrintValue make_value(Type v) {
        PrintValue result;
        result.i32 = v;
        return result;
    }
};
template <>
struct tmp_int_size<2> {
    enum : uint64_t { value = PrintType::Int32 };
    typedef int32_t Type;
    static PrintValue make_value(Type v) {
        PrintValue result;
        result.i32 = v;
        return result;
    }
};
template <>
struct tmp_int_size<4> {
    enum : uint64_t { value = PrintType::Int32 };
    typedef int32_t Type;
    static PrintValue make_value(Type v) {
        PrintValue result;
        result.i32 = v;
        return result;
    }
};
template <>
struct tmp_int_size<8> {
    enum : uint64_t { value = PrintType::Int64 };
    typedef int64_t Type;
    static PrintValue make_value(Type v) {
        PrintValue result;
        result.i64 = v;
        return result;
    }
};
template <size_t N>
struct tmp_uint_size;
template <>
struct tmp_uint_size<1> {
    enum : uint64_t { value = PrintType::UInt32 };
    typedef uint32_t Type;
    static PrintValue make_value(Type v) {
        PrintValue result;
        result.u32 = v;
        return result;
    }
};
template <>
struct tmp_uint_size<2> {
    enum : uint64_t { value = PrintType::UInt32 };
    typedef uint32_t Type;
    static PrintValue make_value(Type v) {
        PrintValue result;
        result.u32 = v;
        return result;
    }
};
template <>
struct tmp_uint_size<4> {
    enum : uint64_t { value = PrintType::UInt32 };
    typedef uint32_t Type;
    static PrintValue make_value(Type v) {
        PrintValue result;
        result.u32 = v;
        return result;
    }
};
template <>
struct tmp_uint_size<8> {
    enum : uint64_t { value = PrintType::UInt64 };
    typedef uint64_t Type;
    static PrintValue make_value(Type v) {
        PrintValue result;
        result.u64 = v;
        return result;
    }
};

template <class... Types>
struct tmp_type_flags<signed char, Types...> {
    enum : uint64_t {
        value = tmp_int_size<sizeof(signed char)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<unsigned char, Types...> {
    enum : uint64_t {
        value = tmp_uint_size<sizeof(unsigned char)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<short, Types...> {
    enum : uint64_t {
        value = tmp_int_size<sizeof(short)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<unsigned short, Types...> {
    enum : uint64_t {
        value = tmp_uint_size<sizeof(unsigned short)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<long, Types...> {
    enum : uint64_t {
        value = tmp_int_size<sizeof(long)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<unsigned long, Types...> {
    enum : uint64_t {
        value = tmp_uint_size<sizeof(unsigned long)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<int, Types...> {
    enum : uint64_t { value = tmp_int_size<sizeof(int)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits) };
};
template <class... Types>
struct tmp_type_flags<unsigned int, Types...> {
    enum : uint64_t {
        value = tmp_uint_size<sizeof(unsigned int)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<long long, Types...> {
    enum : uint64_t {
        value = tmp_int_size<sizeof(long long)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<unsigned long long, Types...> {
    enum : uint64_t {
        value = tmp_uint_size<sizeof(unsigned long long)>::value | (tmp_type_flags<Types...>::value << PrintType::Bits)
    };
};

template <class... Types>
struct tmp_type_flags<bool, Types...> {
    enum : uint64_t { value = PrintType::Bool | (tmp_type_flags<Types...>::value << PrintType::Bits) };
};
template <class... Types>
struct tmp_type_flags<float, Types...> {
    enum : uint64_t { value = PrintType::Float | (tmp_type_flags<Types...>::value << PrintType::Bits) };
};
template <class... Types>
struct tmp_type_flags<double, Types...> {
    enum : uint64_t { value = PrintType::Double | (tmp_type_flags<Types...>::value << PrintType::Bits) };
};
template <class... Types>
struct tmp_type_flags<const char*, Types...> {
    enum : uint64_t { value = PrintType::String | (tmp_type_flags<Types...>::value << PrintType::Bits) };
};
template <class... Types>
struct tmp_type_flags<char*, Types...> {
    enum : uint64_t { value = PrintType::String | (tmp_type_flags<Types...>::value << PrintType::Bits) };
};
#ifdef TM_STRING_VIEW
template <class... Types>
struct tmp_type_flags<TM_STRING_VIEW, Types...> {
    enum : uint64_t { value = PrintType::StringView | (tmp_type_flags<Types...>::value << PrintType::Bits) };
};
#endif
#ifdef TMP_CUSTOM_PRINTING
template <class T, class... Types>
struct tmp_type_flags<T, Types...> {
    enum : uint64_t { value = PrintType::Custom | (tmp_type_flags<Types...>::value << PrintType::Bits) };
};
#else
template <class T, class... Types>
struct tmp_type_flags<T, Types...> {
    enum : uint64_t { value = 0 };
};
#endif

template <class... Types>
void fill_print_arg_list(PrintArgList* list, char value, const Types&... args) {
    list->args[list->size++].c = value;
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, signed char value, const Types&... args) {
    typedef tmp_int_size<sizeof(signed char)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, unsigned char value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned char)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, short value, const Types&... args) {
    typedef tmp_int_size<sizeof(short)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, unsigned short value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned short)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, long value, const Types&... args) {
    typedef tmp_int_size<sizeof(long)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, unsigned long value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned long)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, bool value, const Types&... args) {
    list->args[list->size++].b = value;
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, int value, const Types&... args) {
    typedef tmp_int_size<sizeof(int)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, unsigned int value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned int)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, long long value, const Types&... args) {
    typedef tmp_int_size<sizeof(long long)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, unsigned long long value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned long long)> Print;
    list->args[list->size++] = Print::make_value((Print::Type)value);
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, float value, const Types&... args) {
    list->args[list->size++].f = value;
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, double value, const Types&... args) {
    list->args[list->size++].d = value;
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, const char* value, const Types&... args) {
    list->args[list->size++].s = value;
    fill_print_arg_list(list, args...);
}
template <class... Types>
void fill_print_arg_list(PrintArgList* list, char* value, const Types&... args) {
    list->args[list->size++].s = value;
    fill_print_arg_list(list, args...);
}
#ifdef TM_STRING_VIEW
template <class... Types>
void fill_print_arg_list(PrintArgList* list, TM_STRING_VIEW value, const Types&... args) {
    list->args[list->size].v.data = TM_STRING_VIEW_DATA(value);
    list->args[list->size].v.size = TM_STRING_VIEW_SIZE(value);
    ++list->size;
    fill_print_arg_list(list, args...);
}
#endif
#ifdef TMP_CUSTOM_PRINTING
template <class T, class... Types>
void fill_print_arg_list(PrintArgList* list, const T& value, const Types&... args);
#else
template <class T, class... Types>
void fill_print_arg_list(PrintArgList*, const T&, const Types&...) {
    static_assert(tmp_type_flags<T>::value != 0,
                  "T is not printable, custom printing is disabled (TMP_CUSTOM_PRINTING not defined)");
    static_assert(tmp_type_flags<T>::value == 0, "");  // this function is not allowed to be instantiated
}
#endif
inline void fill_print_arg_list(PrintArgList*) {}

template <class... Types>
void make_print_arg_list(PrintArgList* list, size_t capacity, const Types&... args) {
    list->flags = tmp_type_flags<typename std::decay<Types>::type...>::value;
    list->size = 0;
    fill_print_arg_list(list, args...);
    (void)capacity;
    TM_ASSERT(list->size == capacity);
}
template <class... Types>
void make_print_arg_list(PrintArgList* list, size_t capacity) {
    list->flags = 0;
    list->size = 0;
    (void)capacity;
    TM_ASSERT(list->size == capacity);
}

// Wrapper to an arguments list array on the stack.
// The array size is a ternary expression so that the argument list also works with zero arguments.
#define TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args)                                              \
    static_assert(sizeof...(args) <= PrintType::Count, "Invalid number of arguments to print"); \
    PrintValue values[sizeof...(args) ? sizeof...(args) : 1];                                   \
    PrintArgList arg_list = {values, /*flags=*/0, /*size=*/0};                                  \
    make_print_arg_list(&arg_list, sizeof...(args), args...);

#ifndef TMP_NO_CRT_FILE_PRINTING
template <class... Types>
tm_errc print(const char* format, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_print(stdout, format, TM_STRLEN(format), default_print_format(), arg_list);
}
template <class... Types>
tm_errc print(FILE* out, const char* format, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_print(out, format, TM_STRLEN(format), default_print_format(), arg_list);
}
template <class... Types>
tm_errc print(const char* format, const PrintFormat& initial_formatting, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_print(stdout, format, TM_STRLEN(format), initial_formatting, arg_list);
}
template <class... Types>
tm_errc print(FILE* out, const char* format, const PrintFormat& initial_formatting, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_print(out, format, TM_STRLEN(format), initial_formatting, arg_list);
}
// impl

#ifdef TM_STRING_VIEW
template <class... Types>
tm_errc print(TM_STRING_VIEW format, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_print(stdout, format, TM_STRLEN(format), default_print_format(), arg_list);
}
template <class... Types>
tm_errc print(FILE* out, TM_STRING_VIEW format, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_print(out, format, TM_STRLEN(format), default_print_format(), arg_list);
}
template <class... Types>
tm_errc print(TM_STRING_VIEW format, const PrintFormat& initial_formatting, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_print(stdout, TM_STRING_VIEW_DATA(format), TM_STRING_VIEW_SIZE(format), initial_formatting, arg_list);
}
template <class... Types>
tm_errc print(FILE* out, TM_STRING_VIEW format, const PrintFormat& initial_formatting, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_print(out, TM_STRING_VIEW_DATA(format), TM_STRING_VIEW_SIZE(format), initial_formatting, arg_list);
}
#endif  // defined(TM_STRING_VIEW)
#endif  // TMP_NO_CRT_FILE_PRINTING

template <class... Types>
int snprint(char* dest, tm_size_t len, const char* format, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_snprint(dest, len, format, TM_STRLEN(format), default_print_format(), arg_list);
}
template <class... Types>
int snprint(char* dest, tm_size_t len, const char* format, const PrintFormat& initial_formatting,
            const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_snprint(dest, len, format, TM_STRLEN(format), initial_formatting, arg_list);
}
#ifdef TM_STRING_VIEW
template <class... Types>
int snprint(char* dest, tm_size_t len, TM_STRING_VIEW format, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_snprint(dest, len, TM_STRING_VIEW_DATA(format), TM_STRING_VIEW_SIZE(format), default_print_format(),
                       arg_list);
}
template <class... Types>
int snprint(char* dest, tm_size_t len, TM_STRING_VIEW format, const PrintFormat& initial_formatting,
            const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_snprint(dest, len, TM_STRING_VIEW_DATA(format), TM_STRING_VIEW_SIZE(format), initial_formatting,
                       arg_list);
}
#endif  // defined(TM_STRING_VIEW)

#ifdef TMP_USE_STL
template <class... Types>
::std::string string_format(const char* format, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_string_format(format, TM_STRLEN(format), default_print_format(), arg_list);
}
#ifdef TM_STRING_VIEW
template <class... Types>
::std::string string_format(TM_STRING_VIEW format, const Types&... args) {
    TMP_INTERNAL_MAKE_ARG_LIST(arg_list, args);
    return tmp_string_format(TM_STRING_VIEW_DATA(format), TM_STRING_VIEW_SIZE(format), default_print_format(),
                             arg_list);
}
#endif  // defined(TM_STRING_VIEW)
#endif  // defined(TMP_USE_STL)

#undef TMP_INTERNAL_MAKE_ARG_LIST

#ifdef TMP_CUSTOM_PRINTING
// This looks very confusing, but it checks for the existence of a specific overload of snprint.
// This way we can do a static_assert on whether the overload exists and report an error otherwise.
template <class T>
class tmp_has_custom_printer {
    typedef char no;

    template <class C>
    static auto test(C c)
        -> decltype(static_cast<int (*)(char*, tm_size_t, const ::tml::PrintFormat&, const C&)>(&snprint));
    template <class C>
    static no test(...);

   public:
    enum { value = (sizeof(test<T>(T{})) == sizeof(void*)) };
};

template <class T, class... Types>
void fill_print_arg_list(PrintArgList* list, const T& value, const Types&... args) {
    static_assert(tmp_has_custom_printer<T>::value,
                  "T is not printable, there is no snprint that takes value of type T");
    // If the static assert fails, the compiler will also report that there are no overloads of snprint that accept the
    // argument types. We could get rid of that error by using SFINAE but that introduces more boilerplate.

    // Having constexpr if simplifies the error message.
#ifdef TMP_HAS_CONSTEXPR_IF
    if constexpr (tmp_has_custom_printer<T>::value)
#endif
    {
        auto custom = &list->args[list->size++].custom;
        custom->data = &value;
        custom->customPrint = [](char* buffer, tm_size_t len, const PrintFormat& initial_formatting, const void* data) {
            return snprint(buffer, len, initial_formatting, *(const T*)data);
        };
    }
    fill_print_arg_list(list, args...);
}
#endif  // defined(TMP_CUSTOM_PRINTING)