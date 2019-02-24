template <class... Types>
struct tmp_type_flags;

template <>
struct tmp_type_flags<> {
    enum : uint64_t { Value = 0 };
};

template <class... Types>
struct tmp_type_flags<char, Types...> {
    enum : uint64_t { Value = PrintType::Char | (tmp_type_flags<Types...>::Value << PrintType::Bits) };
};

// integer types to enum mapping based on their size
template <size_t N>
struct tmp_int_size;

template <>
struct tmp_int_size<1> {
    enum : uint64_t { Value = PrintType::Int32 };
    typedef int32_t Type;
    static PrintValue makeValue(Type v) {
        PrintValue result;
        result.i32 = v;
        return result;
    }
};
template <>
struct tmp_int_size<2> {
    enum : uint64_t { Value = PrintType::Int32 };
    typedef int32_t Type;
    static PrintValue makeValue(Type v) {
        PrintValue result;
        result.i32 = v;
        return result;
    }
};
template <>
struct tmp_int_size<4> {
    enum : uint64_t { Value = PrintType::Int32 };
    typedef int32_t Type;
    static PrintValue makeValue(Type v) {
        PrintValue result;
        result.i32 = v;
        return result;
    }
};
template <>
struct tmp_int_size<8> {
    enum : uint64_t { Value = PrintType::Int64 };
    typedef int64_t Type;
    static PrintValue makeValue(Type v) {
        PrintValue result;
        result.i64 = v;
        return result;
    }
};
template <size_t N>
struct tmp_uint_size;
template <>
struct tmp_uint_size<1> {
    enum : uint64_t { Value = PrintType::UInt32 };
    typedef uint32_t Type;
    static PrintValue makeValue(Type v) {
        PrintValue result;
        result.u32 = v;
        return result;
    }
};
template <>
struct tmp_uint_size<2> {
    enum : uint64_t { Value = PrintType::UInt32 };
    typedef uint32_t Type;
    static PrintValue makeValue(Type v) {
        PrintValue result;
        result.u32 = v;
        return result;
    }
};
template <>
struct tmp_uint_size<4> {
    enum : uint64_t { Value = PrintType::UInt32 };
    typedef uint32_t Type;
    static PrintValue makeValue(Type v) {
        PrintValue result;
        result.u32 = v;
        return result;
    }
};
template <>
struct tmp_uint_size<8> {
    enum : uint64_t { Value = PrintType::UInt64 };
    typedef uint64_t Type;
    static PrintValue makeValue(Type v) {
        PrintValue result;
        result.u64 = v;
        return result;
    }
};

template <class... Types>
struct tmp_type_flags<signed char, Types...> {
    enum : uint64_t {
        Value = tmp_int_size<sizeof(signed char)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<unsigned char, Types...> {
    enum : uint64_t {
        Value = tmp_uint_size<sizeof(unsigned char)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<short, Types...> {
    enum : uint64_t {
        Value = tmp_int_size<sizeof(short)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<unsigned short, Types...> {
    enum : uint64_t {
        Value = tmp_uint_size<sizeof(unsigned short)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<long, Types...> {
    enum : uint64_t {
        Value = tmp_int_size<sizeof(long)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<unsigned long, Types...> {
    enum : uint64_t {
        Value = tmp_uint_size<sizeof(unsigned long)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<int, Types...> {
    enum : uint64_t { Value = tmp_int_size<sizeof(int)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits) };
};
template <class... Types>
struct tmp_type_flags<unsigned int, Types...> {
    enum : uint64_t {
        Value = tmp_uint_size<sizeof(unsigned int)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<long long, Types...> {
    enum : uint64_t {
        Value = tmp_int_size<sizeof(long long)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits)
    };
};
template <class... Types>
struct tmp_type_flags<unsigned long long, Types...> {
    enum : uint64_t {
        Value = tmp_uint_size<sizeof(unsigned long long)>::Value | (tmp_type_flags<Types...>::Value << PrintType::Bits)
    };
};

template <class... Types>
struct tmp_type_flags<bool, Types...> {
    enum : uint64_t { Value = PrintType::Bool | (tmp_type_flags<Types...>::Value << PrintType::Bits) };
};
template <class... Types>
struct tmp_type_flags<float, Types...> {
    enum : uint64_t { Value = PrintType::Float | (tmp_type_flags<Types...>::Value << PrintType::Bits) };
};
template <class... Types>
struct tmp_type_flags<double, Types...> {
    enum : uint64_t { Value = PrintType::Double | (tmp_type_flags<Types...>::Value << PrintType::Bits) };
};
template <class... Types>
struct tmp_type_flags<const char*, Types...> {
    enum : uint64_t { Value = PrintType::String | (tmp_type_flags<Types...>::Value << PrintType::Bits) };
};
#ifdef TM_STRING_VIEW
template <class... Types>
struct tmp_type_flags<TM_STRING_VIEW, Types...> {
    enum : uint64_t { Value = PrintType::StringView | (tmp_type_flags<Types...>::Value << PrintType::Bits) };
};
#endif
#ifdef TMP_CUSTOM_PRINTING
template <class T, class... Types>
struct tmp_type_flags<T, Types...> {
    enum : uint64_t { Value = PrintType::Custom | (tmp_type_flags<Types...>::Value << PrintType::Bits) };
};
#else
template <class T, class... Types>
struct tmp_type_flags<T, Types...> {
    enum : uint64_t { Value = 0 };
};
#endif

template <class... Types>
void fillPrintArgList(PrintArgList* list, char value, const Types&... args) {
    list->args[list->size++].c = value;
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, signed char value, const Types&... args) {
    typedef tmp_int_size<sizeof(signed char)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, unsigned char value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned char)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, short value, const Types&... args) {
    typedef tmp_int_size<sizeof(short)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, unsigned short value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned short)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, long value, const Types&... args) {
    typedef tmp_int_size<sizeof(long)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, unsigned long value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned long)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, bool value, const Types&... args) {
    list->args[list->size++].b = value;
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, int value, const Types&... args) {
    typedef tmp_int_size<sizeof(int)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, unsigned int value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned int)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, long long value, const Types&... args) {
    typedef tmp_int_size<sizeof(long long)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, unsigned long long value, const Types&... args) {
    typedef tmp_uint_size<sizeof(unsigned long long)> Print;
    list->args[list->size++] = Print::makeValue((Print::Type)value);
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, float value, const Types&... args) {
    list->args[list->size++].f = value;
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, double value, const Types&... args) {
    list->args[list->size++].d = value;
    fillPrintArgList(list, args...);
}
template <class... Types>
void fillPrintArgList(PrintArgList* list, const char* value, const Types&... args) {
    list->args[list->size++].s = value;
    fillPrintArgList(list, args...);
}
#ifdef TM_STRING_VIEW
template <class... Types>
void fillPrintArgList(PrintArgList* list, TM_STRING_VIEW value, const Types&... args) {
    list->args[list->size].v.data = TM_STRING_VIEW_DATA(value);
    list->args[list->size].v.size = TM_STRING_VIEW_SIZE(value);
    ++list->size;
    fillPrintArgList(list, args...);
}
#endif
#ifdef TMP_CUSTOM_PRINTING
template <class T, class... Types>
void fillPrintArgList(PrintArgList* list, const T& value, const Types&... args);
#else
template <class T, class... Types>
void fillPrintArgList(PrintArgList*, const T&, const Types&...) {
    static_assert(tmp_type_flags<T>::Value != 0,
                  "T is not printable, custom printing is disabled (TMP_CUSTOM_PRINTING not defined)");
    static_assert(tmp_type_flags<T>::Value == 0, "");  // this function is not allowed to be instantiated
}
#endif
void fillPrintArgList(PrintArgList*) {}

template <class... Types>
void makePrintArgList(PrintArgList* list, size_t capacity, const Types&... args) {
    list->flags = tmp_type_flags<Types...>::Value;
    list->size = 0;
    fillPrintArgList(list, args...);
    (void)capacity;
    TM_ASSERT(list->size == capacity);
}

#ifndef TMP_NO_STDIO
template <class... Types>
tm_errc print(const char* format, const Types&... args) {
    static_assert(sizeof...(args) <= PrintType::Count, "Invalid number of arguments to print");
    PrintValue values[sizeof...(args)];
    PrintArgList argList = {values, /*flags=*/0, /*size=*/0};
    makePrintArgList(&argList, sizeof...(args), args...);
    return tmp_print(stdout, format, argList);
}
template <class... Types>
tm_errc print(FILE* out, const char* format, const Types&... args) {
    static_assert(sizeof...(args) <= PrintType::Count, "Invalid number of arguments to print");
    PrintValue values[sizeof...(args)];
    PrintArgList argList = {values, /*flags=*/0, /*size=*/0};
    makePrintArgList(&argList, sizeof...(args), args...);
    return tmp_print(out, format, argList);
}
// impl

#ifdef TM_STRING_VIEW
template <class... Types>
tm_errc print(TM_STRING_VIEW format, const Types&... args) {
    static_assert(sizeof...(args) <= PrintType::Count, "Invalid number of arguments to print");
    PrintValue values[sizeof...(args)];
    PrintArgList argList = {values, /*flags=*/0, /*size=*/0};
    makePrintArgList(&argList, sizeof...(args), args...);
    return tmp_print(stdout, format, argList);
}
template <class... Types>
tm_errc print(FILE* out, TM_STRING_VIEW format, const Types&... args) {
    static_assert(sizeof...(args) <= PrintType::Count, "Invalid number of arguments to print");
    PrintValue values[sizeof...(args)];
    PrintArgList argList = {values, /*flags=*/0, /*size=*/0};
    makePrintArgList(&argList, sizeof...(args), args...);
    return tmp_print(out, format, argList);
}
#endif  // defined( TM_STRING_VIEW )
#endif  // TMP_NO_STDIO

template <class... Types>
tm_size_t snprint(char* dest, tm_size_t len, const char* format, const Types&... args) {
    static_assert(sizeof...(args) <= PrintType::Count, "Invalid number of arguments to snprint");
    PrintValue values[sizeof...(args)];
    PrintArgList argList = {values, /*flags=*/0, /*size=*/0};
    makePrintArgList(&argList, sizeof...(args), args...);
    return tmp_snprint(dest, len, format, defaultPrintFormat(), argList);
}
template <class... Types>
tm_size_t snprint(char* dest, tm_size_t len, const char* format, const PrintFormat& initialFormatting,
                  const Types&... args) {
    static_assert(sizeof...(args) <= PrintType::Count, "Invalid number of arguments to snprint");
    PrintValue values[sizeof...(args)];
    PrintArgList argList = {values, /*flags=*/0, /*size=*/0};
    makePrintArgList(&argList, sizeof...(args), args...);
    return tmp_snprint(dest, len, format, initialFormatting, argList);
}
#ifdef TM_STRING_VIEW
template <class... Types>
tm_size_t snprint(char* dest, tm_size_t len, TM_STRING_VIEW format, const Types&... args) {
    static_assert(sizeof...(args) <= PrintType::Count, "Invalid number of arguments to snprint");
    PrintValue values[sizeof...(args)];
    PrintArgList argList = {values, /*flags=*/0, /*size=*/0};
    makePrintArgList(&argList, sizeof...(args), args...);
    return tmp_snprint(dest, len, format, defaultPrintFormat(), argList);
}
template <class... Types>
tm_size_t snprint(char* dest, tm_size_t len, TM_STRING_VIEW format, const PrintFormat& initialFormatting,
                  const Types&... args) {
    static_assert(sizeof...(args) <= PrintType::Count, "Invalid number of arguments to snprint");
    PrintValue values[sizeof...(args)];
    PrintArgList argList = {values, /*flags=*/0, /*size=*/0};
    makePrintArgList(&argList, sizeof...(args), args...);
    return tmp_snprint(dest, len, format, initialFormatting, argList);
}
#endif  // defined( TM_STRING_VIEW )

#ifdef TMP_CUSTOM_PRINTING
// this looks very confusing, but it checks for the existence of a specific overload of snprint
// this way we can do a static_assert on whether the overload exists and report an error
// otherwise
template <class T>
class tmp_has_custom_printer {
    typedef tm_size_t printer_t(char*, tm_size_t, const PrintFormat&, const T&);
    typedef char no;

    template <class C>
    static auto test(C c)
        -> decltype(static_cast<tm_size_t (*)(char*, tm_size_t, const PrintFormat&, const C&)>(&snprint));
    template <class C>
    static no test(...);

   public:
    enum { Value = (sizeof(test<T>(T{})) == sizeof(void*)) };
};

template <class T, class... Types>
void fillPrintArgList(PrintArgList* list, const T& value, const Types&... args) {
    static_assert(tmp_has_custom_printer<T>::Value,
                  "T is not printable, there is no snprint that takes value of type T");
    // if the static assert fails, the compiler will also report that there are no overloads of
    // snprint that accept the argument types. We could get rid of that error by using SFINAE
    // but that introduces more boilerplate

    // having constexpr if simplifies the error message
#ifdef TMP_HAS_CONSTEXPR_IF
    if constexpr (tmp_has_custom_printer<T>::Value)
#endif
    {
        auto custom = &list->args[list->size++].custom;
        custom->data = &value;
        custom->customPrint = [](char* buffer, tm_size_t len, const PrintFormat& initialFormatting, const void* data) {
            return snprint(buffer, len, initialFormatting, *(const T*)data);
        };
    }
    fillPrintArgList(list, args...);
}
#endif // defined(TMP_CUSTOM_PRINTING)