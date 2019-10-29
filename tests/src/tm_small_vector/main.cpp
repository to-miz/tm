#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <assert_throws.h>
#include <assert_throws.cpp>

// clang-format off
#ifdef USE_SIGNED_SIZE_T
    #define TM_SIZE_T_DEFINED
    #define TM_SIZE_T_IS_SIGNED 1
    typedef int tm_size_t;
#endif
// clang-format on

#include <memory>
#include <redirected_malloc.cpp>

#include <tm_small_vector.h>
using namespace tml;

#include <cstring>

// Testing code starts here.
template <class T>
struct buffer_view {
    T* data;
    tm_size_t size;

    operator buffer_view<const T>() const { return {data, size}; }
};

// Count how many non trivial objects exist to make sure that each constructor is correctly paired to a destructor.
size_t non_trivials_exist = 0;
template <int N>
struct non_trivial_base {
    int8_t values[N];

    non_trivial_base() {
        set_value(-1);
        ++non_trivials_exist;
    }
    explicit non_trivial_base(int v) {
        set_value(v);
        ++non_trivials_exist;
    }
    non_trivial_base(non_trivial_base&& other) {
        memcpy(values, other.values, N * sizeof(int8_t));
        other.set_value(-2);
        ++non_trivials_exist;
    }
    non_trivial_base(const non_trivial_base& other) {
        memcpy(values, other.values, N * sizeof(int8_t));
        ++non_trivials_exist;
    }
    ~non_trivial_base() {
        set_value(-3);
        --non_trivials_exist;
    }
    non_trivial_base& operator=(non_trivial_base&& other) {
        if (this != &other) {
            int8_t temp[N];
            memcpy(temp, values, N * sizeof(int8_t));
            memcpy(values, other.values, N * sizeof(int8_t));
            memcpy(other.values, temp, N * sizeof(int8_t));
        }
        return *this;
    }
    non_trivial_base& operator=(const non_trivial_base& other) {
        if (this != &other) memcpy(values, other.values, N * sizeof(int8_t));
        return *this;
    }

    void set_value(int v) {
        for (auto& entry : values) {
            entry = (int8_t)v;
        }
    }

    friend bool operator!=(non_trivial_base lhs, non_trivial_base rhs) {
        return memcmp(lhs.values, rhs.values, N * sizeof(int8_t)) != 0;
    }
    friend bool operator!=(int lhs, non_trivial_base rhs) {
        for (auto entry : rhs.values) {
            if (entry != lhs) return true;
        }
        return false;
    }
    friend bool operator!=(non_trivial_base lhs, int rhs) {
        for (auto entry : lhs.values) {
            if (entry != lhs) return true;
        }
        return false;
    }
};
typedef non_trivial_base<4> non_trivial;
typedef non_trivial_base<8> non_trivial8;
typedef non_trivial_base<12> non_trivial12;
typedef non_trivial_base<24> non_trivial24;
typedef non_trivial_base<36> non_trivial36;
typedef non_trivial_base<72> non_trivial72;
template <int N>
std::ostream& operator<<(std::ostream& o, non_trivial_base<N> v) {
    o << v.values[0];
    return o;
}
TYPE_TO_STRING(non_trivial);

struct non_trivial_guard {
    size_t prev_non_trivials_exist;
    non_trivial_guard() : prev_non_trivials_exist(non_trivials_exist) {}
    ~non_trivial_guard() { CHECK(prev_non_trivials_exist == non_trivials_exist); }
};

template <class T, tm_size_t N, class AllocatorTag>
std::ostream& operator<<(std::ostream& o, const small_vector<T, N, AllocatorTag>& v) {
    o << "[";
    for (tm_size_t i = 0, count = v.size(); i < count; ++i) {
        o << v[i];
        if (i + 1 != count) o << ", ";
    }
    o << "]";
    return o;
}
template <class T>
std::ostream& operator<<(std::ostream& o, const std::initializer_list<T>& v) {
    o << "[";
    auto p = v.begin();
    for (size_t i = 0, count = v.size(); i < count; ++i) {
        o << p[i];
        if (i + 1 != count) o << ", ";
    }
    o << "]";
    return o;
}
template <class T>
std::ostream& operator<<(std::ostream& o, const buffer_view<T>& v) {
    o << "[";
    auto p = v.data;
    for (tm_size_t i = 0, count = v.size; i < count; ++i) {
        o << p[i];
        if (i + 1 != count) o << ", ";
    }
    o << "]";
    return o;
}

template <class T, tm_size_t N0, class AllocatorTag0, tm_size_t N1, class AllocatorTag1>
bool equals(const small_vector<T, N0, AllocatorTag0>& lhs, const small_vector<T, N0, AllocatorTag0>& rhs) {
    if (&lhs == &rhs) return true;
    if (lhs.size() != rhs.size()) return false;
    for (tm_size_t i = 0, count = lhs.size(); i < count; ++i) {
        if (lhs[i] != rhs[i]) return false;
    }
    return true;
}
template <class T, class AllocatorTag>
bool equals_list(const small_vector_base<T, AllocatorTag>& lhs, std::initializer_list<T> rhs) {
    if (lhs.size() != (tm_size_t)rhs.size()) return false;
    auto p = rhs.begin();
    for (tm_size_t i = 0, count = lhs.size(); i < count; ++i) {
        if (lhs[i] != p[i]) return false;
    }
    return true;
}
template <class T, tm_size_t N, class AllocatorTag, class V>
bool equals_view(const small_vector<T, N, AllocatorTag>& lhs, buffer_view<V> rhs) {
    if (lhs.size() != rhs.size) return false;
    for (tm_size_t i = 0, count = lhs.size(); i < count; ++i) {
        if (lhs[i] != rhs.data[i]) return false;
    }
    return true;
}
template <class T, tm_size_t N0, class AllocatorTag0, tm_size_t N1, class AllocatorTag1>
bool operator==(const small_vector<T, N0, AllocatorTag0>& lhs, const small_vector<T, N0, AllocatorTag0>& rhs) {
    return equals(lhs, rhs);
}
template <class T, tm_size_t N, class AllocatorTag>
bool operator==(const small_vector<T, N, AllocatorTag>& lhs, const std::initializer_list<T>& rhs) {
    return equals_list(lhs, rhs);
}

TEST_CASE_TEMPLATE("push_back", T, int, non_trivial, non_trivial8, non_trivial12, non_trivial24, non_trivial36,
                   non_trivial72) {
    allocation_guard guard{};
    non_trivial_guard non_trivial_destructors_guard{};

    small_vector<T, 3> v;
    T buffer[10];
    for (int i = 0; i < 10; ++i) {
        buffer[i] = T{i};
        v.push_back(T{i});
        REQUIRE(equals_view(v, buffer_view<T>{buffer, (tm_size_t)i + 1}));
    }
}

TEST_CASE_TEMPLATE("pop_back", T, int, non_trivial, non_trivial8, non_trivial12, non_trivial24, non_trivial36,
                   non_trivial72) {
    allocation_guard guard{};
    non_trivial_guard non_trivial_destructors_guard{};

    small_vector<T, 3> v;
    T buffer[10];
    for (int i = 0; i < 10; ++i) {
        buffer[i] = T{i};
        v.push_back(T{i});
        REQUIRE(equals_view(v, buffer_view<T>{buffer, (tm_size_t)i + 1}));
    }

    for (int i = 10; i > 0; --i) {
        v.pop_back();
        REQUIRE(equals_view(v, buffer_view<T>{buffer, (tm_size_t)i - 1}));
    }
}

TEST_CASE_TEMPLATE("insert end", T, int, non_trivial, non_trivial8, non_trivial12, non_trivial24, non_trivial36,
                   non_trivial72) {
    allocation_guard guard{};
    non_trivial_guard non_trivial_destructors_guard{};

    SUBCASE("single") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 3> v;
        T buffer[10];
        for (int i = 0; i < 10; ++i) {
            buffer[i] = T{i};
            v.insert(v.end(), T{i});
            REQUIRE(equals_view(v, buffer_view<T>{buffer, (tm_size_t)i + 1}));
        }
    }
    SUBCASE("multiple") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 3> v;
        T buffer[20];
        buffer_view<T> view = {};

        CAPTURE(view);
        CAPTURE(v);

        for (int i = 0; i < 10; i += 2) {
            buffer[i] = T{i};
            buffer[i + 1] = T{i};
            v.insert(v.end(), 2, T{i});
            view = {buffer, (tm_size_t)i + 2};
            REQUIRE(equals_view(v, view));
        }
    }
    SUBCASE("range sbo") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 10> v;
        v.assign({T{0}, T{1}, T{2}, T{4}});
        v.insert(v.end(), {T{10}, T{11}, T{12}});
        REQUIRE(equals_list(v, {T{0}, T{1}, T{2}, T{4}, T{10}, T{11}, T{12}}));
    }
    SUBCASE("range dynamic before insert") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 1> v;
        v.assign({T{0}, T{1}, T{2}, T{4}});
        v.insert(v.end(), {T{10}, T{11}, T{12}});
        REQUIRE(equals_list(v, {T{0}, T{1}, T{2}, T{4}, T{10}, T{11}, T{12}}));
    }
    SUBCASE("range dynamic after insert") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> v;
        v.assign({T{0}, T{1}, T{2}, T{4}});
        v.insert(v.end(), {T{10}, T{11}, T{12}});
        REQUIRE(equals_list(v, {T{0}, T{1}, T{2}, T{4}, T{10}, T{11}, T{12}}));
    }
    SUBCASE("self insert") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> v;
        v.assign({T{0}, T{1}, T{2}});
        v.insert(v.end(), v[0]);
        REQUIRE(equals_list(v, {T{0}, T{1}, T{2}, T{0}}));
        CHECK_ASSERTION_FAILURE(v.insert(v.end(), v.begin(), v.end()));
    }
}

TEST_CASE_TEMPLATE("insert beginning", T, int, non_trivial, non_trivial8, non_trivial12, non_trivial24, non_trivial36,
                   non_trivial72) {
    allocation_guard guard{};
    non_trivial_guard non_trivial_destructors_guard{};

    SUBCASE("single") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 3> v;
        const int size = 10;
        T buffer[size];
        T* p = buffer + size - 1;
        for (int i = 0; i < size; ++i) {
            *p = T{i};
            v.insert(v.begin(), T{i});
            buffer_view<T> view = {p, (tm_size_t)i + 1};
            --p;

            REQUIRE(equals_view(v, view));
        }
    }
    SUBCASE("multiple") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 3> v;
        T buffer[20];
        buffer_view<T> view = {};

        CAPTURE(view);
        CAPTURE(v);

        T* p = buffer + 20 - 2;
        for (int i = 0; i < 10; i += 2) {
            *p = T{i};
            *(p + 1) = T{i};
            v.insert(v.begin(), 2, T{i});
            view = {p, (tm_size_t)i + 2};
            p -= 2;
            REQUIRE(equals_view(v, view));
        }
    }
    SUBCASE("range sbo") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 10> v;
        v.assign({T{0}, T{1}, T{2}, T{4}});
        v.insert(v.begin(), {T{10}, T{11}, T{12}});
        REQUIRE(equals_list(v, {T{10}, T{11}, T{12}, T{0}, T{1}, T{2}, T{4}}));
    }
    SUBCASE("range dynamic before insert") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 1> v;
        v.assign({T{0}, T{1}, T{2}, T{4}});
        v.insert(v.begin(), {T{10}, T{11}, T{12}});
        REQUIRE(equals_list(v, {T{10}, T{11}, T{12}, T{0}, T{1}, T{2}, T{4}}));
    }
    SUBCASE("range dynamic after insert") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> v;
        v.assign({T{0}, T{1}, T{2}, T{4}});
        v.insert(v.begin(), {T{10}, T{11}, T{12}});
        REQUIRE(equals_list(v, {T{10}, T{11}, T{12}, T{0}, T{1}, T{2}, T{4}}));
    }
    SUBCASE("self insert") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> v;
        v.assign({T{0}, T{1}, T{2}});
        v.insert(v.begin(), v[0]);
        REQUIRE(equals_list(v, {T{0}, T{0}, T{1}, T{2}}));
        CHECK_ASSERTION_FAILURE(v.insert(v.begin(), v.begin(), v.end()));
    }
}

TEST_CASE_TEMPLATE("insert middle", T, int, non_trivial, non_trivial8, non_trivial12, non_trivial24, non_trivial36,
                   non_trivial72) {
    allocation_guard guard{};
    non_trivial_guard non_trivial_destructors_guard{};

    SUBCASE("single") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 3> v;
        const int size = 10;
        T buffer[size];
        T* p = buffer;
        for (int i = 0; i < size; ++i) {
            auto index = i / 2;
            memmove(p + index + 1, p + index, (size - index - 1) * sizeof(T));
            p[i / 2] = T{i};

            v.insert(v.begin() + v.size() / 2, T{i});
            buffer_view<T> view = {p, (tm_size_t)i + 1};

            CAPTURE(view);
            CAPTURE(v);
            REQUIRE(equals_view(v, view));
        }
    }
    SUBCASE("multiple") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 3> v;
        T buffer[20];
        buffer_view<T> view = {};

        CAPTURE(view);
        CAPTURE(v);

        T* p = buffer;
        for (int i = 0; i < 10; ++i) {
            memmove(p + i + 2, p + i, i * sizeof(T));
            p[i] = T{i};
            p[i + 1] = T{i};

            v.insert(v.begin() + v.size() / 2, 2, T{i});
            view = {p, (tm_size_t)i * 2 + 2};

            REQUIRE(equals_view(v, view));
        }
    }
    SUBCASE("range sbo") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 10> v;
        v.assign({T{0}, T{1}, T{2}, T{4}});
        v.insert(v.begin() + v.size() / 2, {T{10}, T{11}, T{12}});
        REQUIRE(equals_list(v, {T{0}, T{1}, T{10}, T{11}, T{12}, T{2}, T{4}}));
    }
    SUBCASE("range dynamic before insert") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 1> v;
        v.assign({T{0}, T{1}, T{2}, T{4}});
        v.insert(v.begin() + v.size() / 2, {T{10}, T{11}, T{12}});
        REQUIRE(equals_list(v, {T{0}, T{1}, T{10}, T{11}, T{12}, T{2}, T{4}}));
    }
    SUBCASE("range dynamic after insert") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> v;
        v.assign({T{0}, T{1}, T{2}, T{4}});
        v.insert(v.begin() + v.size() / 2, {T{10}, T{11}, T{12}});
        REQUIRE(equals_list(v, {T{0}, T{1}, T{10}, T{11}, T{12}, T{2}, T{4}}));
    }
    SUBCASE("self insert") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> v;
        v.assign({T{0}, T{1}, T{2}, T{3}});
        v.insert(v.begin() + v.size() / 2, v[0]);
        REQUIRE(equals_list(v, {T{0}, T{1}, T{0}, T{2}, T{3}}));
        CHECK_ASSERTION_FAILURE(v.insert(v.begin() + v.size() / 2, v.begin(), v.end()));
    }
}

TEST_CASE_TEMPLATE("erase beginning", T, int, non_trivial, non_trivial8, non_trivial12, non_trivial24, non_trivial36,
                   non_trivial72) {
    allocation_guard guard{};
    non_trivial_guard non_trivial_destructors_guard{};

    small_vector<T, 3> v;
    std::initializer_list<T> il = {T{1}, T{2}, T{3}, T{4}};
    v.assign(il);
    buffer_view<const T> view = {il.begin(), (tm_size_t)il.size()};

    CAPTURE(v);
    CAPTURE(view);
    while (!v.empty()) {
        REQUIRE(equals_view(v, view));
        v.erase(v.begin());
        view.data++;
        view.size--;
    }
    REQUIRE(equals_view(v, view));
}
TEST_CASE_TEMPLATE("erase last", T, int, non_trivial, non_trivial8, non_trivial12, non_trivial24, non_trivial36,
                   non_trivial72) {
    allocation_guard guard{};
    non_trivial_guard non_trivial_destructors_guard{};

    small_vector<T, 3> v;
    std::initializer_list<T> il = {T{1}, T{2}, T{3}, T{4}};
    v.assign(il);
    buffer_view<const T> view = {il.begin(), (tm_size_t)il.size()};

    CAPTURE(v);
    CAPTURE(view);
    while (!v.empty()) {
        REQUIRE(equals_view(v, view));
        v.erase(v.end() - 1);
        view.size--;
    }
    REQUIRE(equals_view(v, view));
}
TEST_CASE_TEMPLATE("erase middle", T, int, non_trivial, non_trivial8, non_trivial12, non_trivial24, non_trivial36,
                   non_trivial72) {
    allocation_guard guard{};
    non_trivial_guard non_trivial_destructors_guard{};

    small_vector<T, 3> v;
    T buffer[4] = {T{1}, T{2}, T{3}, T{4}};
    v.assign(buffer, buffer + 4);
    buffer_view<T> view = {std::begin(buffer), 4};

    CAPTURE(v);
    CAPTURE(view);
    while (!v.empty()) {
        REQUIRE(equals_view(v, view));
        v.erase(v.begin() + v.size() / 2);
        for (tm_size_t i = view.size / 2; i < view.size - 1; ++i) {
            view.data[i] = view.data[i + 1];
        }
        view.size--;
    }
    REQUIRE(equals_view(v, view));
}
TEST_CASE_TEMPLATE("erase all", T, int, non_trivial, non_trivial8, non_trivial12, non_trivial24, non_trivial36,
                   non_trivial72) {
    allocation_guard guard{};
    non_trivial_guard non_trivial_destructors_guard{};

    small_vector<T, 3> v;
    T buffer[4] = {T{1}, T{2}, T{3}, T{4}};
    v.assign(buffer, buffer + 4);
    buffer_view<T> view = {std::begin(buffer), 4};

    CAPTURE(v);
    CAPTURE(view);
    REQUIRE(equals_view(v, view));

    v.erase(v.begin(), v.end());
    view.size = 0;
    REQUIRE(equals_view(v, view));
}

TEST_CASE_TEMPLATE("erase range", T, int, non_trivial, non_trivial8, non_trivial12, non_trivial24, non_trivial36,
                   non_trivial72) {
    allocation_guard guard{};
    non_trivial_guard non_trivial_destructors_guard{};

    small_vector<T, 3> v;
    T buffer[8] = {T{1}, T{2}, T{3}, T{4}, T{1}, T{2}, T{3}, T{4}};
    T cmp_buffer[8];
    buffer_view<T> view = {};

    CAPTURE(v);
    CAPTURE(view);

    for (int i = 0; i < 5; ++i) {
        v.assign(buffer, buffer + 8);
        v.erase(v.begin() + i, v.begin() + i + 3);
        int index = 0;
        for (int j = 0; j < 8;) {
            if (j == i) {
                j += 3;
                continue;
            }
            cmp_buffer[index++] = buffer[j];
            ++j;
        }
        view.data = cmp_buffer;
        view.size = 5;
        REQUIRE(equals_view(v, view));
    }
}

TEST_CASE_TEMPLATE("emplace_back", T, int, non_trivial, non_trivial8, non_trivial12, non_trivial24, non_trivial36,
                   non_trivial72) {
    allocation_guard guard{};
    non_trivial_guard non_trivial_destructors_guard{};

    small_vector<T, 1> v;
    v.emplace_back(1);
    CHECK(equals_list(v, {T{1}}));
}

TEST_CASE_TEMPLATE("emplace", T, int, non_trivial, non_trivial8, non_trivial12, non_trivial24, non_trivial36,
                   non_trivial72) {
    allocation_guard guard{};
    non_trivial_guard non_trivial_destructors_guard{};

    SUBCASE("beginning") {
        {
            small_vector<T, 10> v;
            v.assign({T{0}, T{1}, T{2}, T{3}, T{4}, T{5}, T{6}});  // sbo
            v.emplace(v.begin(), T{10});
            CHECK(equals_list(v, {T{10}, T{0}, T{1}, T{2}, T{3}, T{4}, T{5}, T{6}}));
        }
        {
            small_vector<T, 1> v;
            v.assign({T{0}, T{1}, T{2}, T{3}, T{4}, T{5}, T{6}});  // dynamic
            v.emplace(v.begin(), T{10});
            CHECK(equals_list(v, {T{10}, T{0}, T{1}, T{2}, T{3}, T{4}, T{5}, T{6}}));
        }
        {
            small_vector<T, 0> v;
            v.assign({T{0}, T{1}, T{2}, T{3}, T{4}, T{5}, T{6}});  // dynamic without sbo
            v.emplace(v.begin(), T{10});
            CHECK(equals_list(v, {T{10}, T{0}, T{1}, T{2}, T{3}, T{4}, T{5}, T{6}}));
        }
    }
    SUBCASE("middle") {
        {
            small_vector<T, 10> v;
            v.assign({T{0}, T{1}, T{2}, T{3}, T{4}, T{5}, T{6}});  // sbo
            v.emplace(v.begin() + v.size() / 2, T{10});
            CHECK(equals_list(v, {T{0}, T{1}, T{2}, T{10}, T{3}, T{4}, T{5}, T{6}}));
        }
        {
            small_vector<T, 1> v;
            v.assign({T{0}, T{1}, T{2}, T{3}, T{4}, T{5}, T{6}});  // dynamic
            v.emplace(v.begin() + v.size() / 2, T{10});
            CHECK(equals_list(v, {T{0}, T{1}, T{2}, T{10}, T{3}, T{4}, T{5}, T{6}}));
        }
        {
            small_vector<T, 0> v;
            v.assign({T{0}, T{1}, T{2}, T{3}, T{4}, T{5}, T{6}});  // dynamic without sbo
            v.emplace(v.begin() + v.size() / 2, T{10});
            CHECK(equals_list(v, {T{0}, T{1}, T{2}, T{10}, T{3}, T{4}, T{5}, T{6}}));
        }
    }
    SUBCASE("last") {
        {
            small_vector<T, 10> v;
            v.assign({T{0}, T{1}, T{2}, T{3}, T{4}, T{5}, T{6}});  // sbo
            v.emplace(v.end(), T{10});
            CHECK(equals_list(v, {T{0}, T{1}, T{2}, T{3}, T{4}, T{5}, T{6}, T{10}}));
        }
        {
            small_vector<T, 1> v;
            v.assign({T{0}, T{1}, T{2}, T{3}, T{4}, T{5}, T{6}});  // dynamic
            v.emplace(v.end(), T{10});
            CHECK(equals_list(v, {T{0}, T{1}, T{2}, T{3}, T{4}, T{5}, T{6}, T{10}}));
        }
        {
            small_vector<T, 0> v;
            v.assign({T{0}, T{1}, T{2}, T{3}, T{4}, T{5}, T{6}});  // dynamic without sbo
            v.emplace(v.end(), T{10});
            CHECK(equals_list(v, {T{0}, T{1}, T{2}, T{3}, T{4}, T{5}, T{6}, T{10}}));
        }
    }
}

TEST_CASE_TEMPLATE("resize", T, int, non_trivial, non_trivial8, non_trivial12, non_trivial24, non_trivial36,
                   non_trivial72) {
    SUBCASE("dynamic") {
        small_vector<T, 1> a;
        a.resize(5);                                       // Resize to dynamic.
        CHECK(equals_list(a, {T{}, T{}, T{}, T{}, T{}}));  // Default constructed. 0 for int, -1 for non_trivial.
    }
    SUBCASE("dynamic without sbo") {
        small_vector<T, 0> a;
        a.resize(5);                                       // Resize to dynamic.
        CHECK(equals_list(a, {T{}, T{}, T{}, T{}, T{}}));  // Default constructed. 0 for int, -1 for non_trivial.
    }
    SUBCASE("sbo") {
        small_vector<T, 10> a;
        a.resize(5);                                       // Resize, but still sbo.
        CHECK(equals_list(a, {T{}, T{}, T{}, T{}, T{}}));  // Default constructed. 0 for int, -1 for non_trivial.
    }
    SUBCASE("dynamic value") {
        small_vector<T, 1> a;
        a.resize(5, T{2});  // Resize to dynamic.
        CHECK(equals_list(a, {T{2}, T{2}, T{2}, T{2}, T{2}}));
    }
    SUBCASE("dynamic value without sbo") {
        small_vector<T, 0> a;
        a.resize(5, T{2});  // Resize to dynamic.
        CHECK(equals_list(a, {T{2}, T{2}, T{2}, T{2}, T{2}}));
    }
    SUBCASE("sbo") {
        small_vector<T, 10> a;
        a.resize(5, T{2});  // Resize, but still sbo.
        CHECK(equals_list(a, {T{2}, T{2}, T{2}, T{2}, T{2}}));
    }
}

TEST_CASE_TEMPLATE("swap", T, int, non_trivial, non_trivial8, non_trivial12, non_trivial24, non_trivial36,
                   non_trivial72) {
    allocation_guard guard{};
    non_trivial_guard non_trivial_destructors_guard{};

    {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 2> a;
        small_vector<T, 5> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({T{1}, T{2}});                                                   // sbo
        b.assign({T{10}, T{9}, T{8}, T{7}, T{6}, T{5}, T{4}, T{3}, T{2}, T{1}});  // dynamic
        swap(a, b);
        REQUIRE(equals_list(a, {T{10}, T{9}, T{8}, T{7}, T{6}, T{5}, T{4}, T{3}, T{2}, T{1}}));
        REQUIRE(equals_list(b, {T{1}, T{2}}));
        swap(a, b);
        REQUIRE(equals_list(a, {T{1}, T{2}}));
        REQUIRE(equals_list(b, {T{10}, T{9}, T{8}, T{7}, T{6}, T{5}, T{4}, T{3}, T{2}, T{1}}));
    }

    {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 2> a;
        small_vector<T, 5> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({T{10}, T{9}, T{8}, T{7}, T{6}, T{5}, T{4}, T{3}, T{2}, T{1}});  // dynamic
        b.assign({T{1}, T{2}});                                                   // sbo
        swap(a, b);
        REQUIRE(equals_list(a, {T{1}, T{2}}));
        REQUIRE(equals_list(b, {T{10}, T{9}, T{8}, T{7}, T{6}, T{5}, T{4}, T{3}, T{2}, T{1}}));
        swap(a, b);
        REQUIRE(equals_list(b, {T{1}, T{2}}));
        REQUIRE(equals_list(a, {T{10}, T{9}, T{8}, T{7}, T{6}, T{5}, T{4}, T{3}, T{2}, T{1}}));
    }

    {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 2> a;
        small_vector<T, 5> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({T{1}, T{2}});                    // sbo
        b.assign({T{8}, T{7}, T{6}, T{5}, T{4}});  // sbo
        swap(a, b);
        REQUIRE(equals_list(a, {T{8}, T{7}, T{6}, T{5}, T{4}}));
        REQUIRE(equals_list(b, {T{1}, T{2}}));
        swap(a, b);
        REQUIRE(equals_list(b, {T{8}, T{7}, T{6}, T{5}, T{4}}));
        REQUIRE(equals_list(a, {T{1}, T{2}}));
    }

    {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> a;
        small_vector<T, 5> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({T{5}, T{4}, T{3}, T{2}, T{1}});  // sbo
        b.assign({T{1}, T{2}, T{3}, T{4}, T{5}});  // sbo
        swap(a, b);
        REQUIRE(equals_list(a, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(equals_list(b, {T{5}, T{4}, T{3}, T{2}, T{1}}));
        swap(a, b);
        REQUIRE(equals_list(b, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(equals_list(a, {T{5}, T{4}, T{3}, T{2}, T{1}}));
    }

    {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 1> a;
        small_vector<T, 1> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({T{5}, T{4}, T{3}, T{2}, T{1}});  // dynamic
        b.assign({T{1}, T{2}, T{3}, T{4}, T{5}});  // dynamic
        swap(a, b);
        REQUIRE(equals_list(a, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(equals_list(b, {T{5}, T{4}, T{3}, T{2}, T{1}}));
        swap(a, b);
        REQUIRE(equals_list(b, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(equals_list(a, {T{5}, T{4}, T{3}, T{2}, T{1}}));
    }
    {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 0> a;
        small_vector<T, 0> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({T{5}, T{4}, T{3}, T{2}, T{1}});  // dynamic
        b.assign({T{1}, T{2}, T{3}, T{4}, T{5}});  // dynamic
        swap(a, b);
        REQUIRE(equals_list(a, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(equals_list(b, {T{5}, T{4}, T{3}, T{2}, T{1}}));
        swap(a, b);
        REQUIRE(equals_list(b, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(equals_list(a, {T{5}, T{4}, T{3}, T{2}, T{1}}));
    }

    {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> a;
        small_vector<T, 5> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({});                              // sbo
        b.assign({T{1}, T{2}, T{3}, T{4}, T{5}});  // sbo
        swap(a, b);
        REQUIRE(equals_list(a, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(b.empty());
        swap(a, b);
        REQUIRE(a.empty());
        REQUIRE(equals_list(b, {T{1}, T{2}, T{3}, T{4}, T{5}}));
    }
    {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> a;
        small_vector<T, 5> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({});  // sbo
        b.assign({});  // sbo
        swap(a, b);
        REQUIRE(a.empty());
        REQUIRE(b.empty());
        swap(a, b);
        REQUIRE(a.empty());
        REQUIRE(b.empty());
    }
    {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 0> a;
        small_vector<T, 0> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({});  // sbo
        b.assign({});  // sbo
        swap(a, b);
        REQUIRE(a.empty());
        REQUIRE(b.empty());
        swap(a, b);
        REQUIRE(a.empty());
        REQUIRE(b.empty());
    }
}

TEST_CASE_TEMPLATE("constructors and assignment", T, int, non_trivial, non_trivial8, non_trivial12, non_trivial24,
                   non_trivial36, non_trivial72) {
    allocation_guard guard{};
    non_trivial_guard non_trivial_destructors_guard{};

    SUBCASE("copy assignment") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> a;
        small_vector<T, 5> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({});                              // sbo
        b.assign({T{1}, T{2}, T{3}, T{4}, T{5}});  // sbo
        a = b;
        REQUIRE(equals_list(a, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(equals_list(b, {T{1}, T{2}, T{3}, T{4}, T{5}}));
    }

    SUBCASE("move assignment") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> a;
        small_vector<T, 5> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({});                              // sbo
        b.assign({T{1}, T{2}, T{3}, T{4}, T{5}});  // sbo
        a = std::move(b);
        REQUIRE(equals_list(a, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(b.empty());
    }

    SUBCASE("copy construct") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> a;
        a.assign({T{1}, T{2}, T{3}, T{4}, T{5}});  // sbo
        small_vector<T, 5> b(a);

        CAPTURE(a);
        CAPTURE(b);

        REQUIRE(equals_list(a, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(equals_list(b, {T{1}, T{2}, T{3}, T{4}, T{5}}));
    }

    SUBCASE("move construct") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> a;
        a.assign({T{1}, T{2}, T{3}, T{4}, T{5}});  // sbo
        small_vector<T, 5> b(std::move(a));

        CAPTURE(a);
        CAPTURE(b);

        REQUIRE(a.empty());
        REQUIRE(equals_list(b, {T{1}, T{2}, T{3}, T{4}, T{5}}));
    }
}

TEST_CASE_TEMPLATE("constructors and assignment with base vector", T, int, non_trivial, non_trivial8, non_trivial12,
                   non_trivial24, non_trivial36, non_trivial72) {
    allocation_guard guard{};
    non_trivial_guard non_trivial_destructors_guard{};

    SUBCASE("copy assignment") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector_base<T> a;
        small_vector_base<T> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({});
        b.assign({T{1}, T{2}, T{3}, T{4}, T{5}});
        a = b;
        REQUIRE(equals_list(a, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(equals_list(b, {T{1}, T{2}, T{3}, T{4}, T{5}}));
    }

    SUBCASE("move assignment") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector_base<T> a;
        small_vector_base<T> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({});
        b.assign({T{1}, T{2}, T{3}, T{4}, T{5}});
        a = std::move(b);
        REQUIRE(equals_list(a, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(b.empty());
    }

    SUBCASE("copy construct") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector_base<T> a;
        a.assign({T{1}, T{2}, T{3}, T{4}, T{5}});
        small_vector_base<T> b(a);

        CAPTURE(a);
        CAPTURE(b);

        REQUIRE(equals_list(a, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(equals_list(b, {T{1}, T{2}, T{3}, T{4}, T{5}}));
    }

    SUBCASE("move construct") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector_base<T> a;
        a.assign({T{1}, T{2}, T{3}, T{4}, T{5}});
        small_vector_base<T> b(std::move(a));

        CAPTURE(a);
        CAPTURE(b);

        REQUIRE(a.empty());
        REQUIRE(equals_list(b, {T{1}, T{2}, T{3}, T{4}, T{5}}));
    }
}

struct TestMemory {
    void* memory;
    TestMemory() { memory = malloc(1024 * 1014); }
    ~TestMemory() { free(memory); }
};

TestMemory memory;
TEST_CASE_TEMPLATE("write outside of range test", T, int, non_trivial, non_trivial8, non_trivial12, non_trivial24,
                   non_trivial36, non_trivial72) {
    REQUIRE(memory.memory);
    size_t remaining = 1024 * 1014;
    void* current = memory.memory;
    int allocation_id = 0xFF;
    struct Allocation {
        void* ptr;
        size_t size;
        size_t total_size;
        int id;
        bool freed;
    };
    Allocation allocations[32] = {};
    int allocations_count = 0;

    auto find_allocation = [&](void* ptr) -> Allocation* {
        for (int i = 0; i < allocations_count; ++i) {
            if (allocations[i].ptr == ptr) return &allocations[i];
        }
        return nullptr;
    };

    auto my_malloc = [&](size_t size) -> void* {
        void* success = std::align(/*alignment=*/16, size * 2, current, remaining);
        REQUIRE(success);
        void* result = current;
        current = (char*)current + size * 2;
        remaining -= size * 2;
        memset(result, allocation_id, size * 2);
        REQUIRE(allocations_count < 32);
        allocations[allocations_count++] = {result, size, size * 2, allocation_id, /*freed=*/false};
        --allocation_id;
        return result;
    };
    auto my_realloc = [&](void* ptr, size_t old_size, size_t new_size) -> void* {
        auto result = my_malloc(new_size);
        if (ptr) {
            auto allocation = find_allocation(ptr);
            REQUIRE(allocation);
            REQUIRE(!allocation->freed);
            REQUIRE(allocation->size == old_size);
            allocation->freed = (new_size == 0);
            if (result) {
                allocation->freed = true;
                size_t min_size = (new_size < old_size) ? new_size : old_size;
                if (min_size) memcpy(result, ptr, min_size);
            }
            if (allocation->freed) {
                memset(ptr, allocation->id, allocation->total_size);
            }
        }
        return result;
    };
    auto my_realloc_in_place = [&](void* ptr, size_t old_size, size_t new_size) -> void* {
        if (ptr && new_size < old_size) {
            auto allocation = find_allocation(ptr);
            REQUIRE(allocation);
            REQUIRE(allocation->size == old_size);
            REQUIRE(!allocation->freed);
            memset((char*)ptr + new_size, allocation->id, old_size - new_size);
            return ptr;
        }
        return nullptr;
    };
    auto my_free = [&](void* ptr, size_t size) -> void {
        if (ptr) {
            auto allocation = find_allocation(ptr);
            REQUIRE(allocation);
            REQUIRE(!allocation->freed);
            REQUIRE(allocation->size == size);
            memset(ptr, allocation->id, allocation->total_size);
            allocation->freed = true;
        }
    };

    auto not_clobbered = [&](small_vector_base<T>& v) {
        if (!v.data()) return;
        if (v.is_sbo()) return;

        auto allocation = find_allocation(v.data());
        REQUIRE(allocation);
        REQUIRE(!allocation->freed);
        // Test that all data outside of vector was never written to.
        for (char *c = (char*)(v.data() + v.capacity()), *last = (char*)allocation->ptr + allocation->size; c < last;
             ++c) {
            REQUIRE(*c == allocation->id);
        }
    };

    auto guard = redirect_guard{my_malloc, my_realloc, my_realloc_in_place, my_free};
    allocation_guard alloc_guard{};
    non_trivial_guard non_trivial_destructors_guard{};

    SUBCASE("push_back") {
        small_vector<T, 3> v;
        T buffer[10];
        for (int i = 0; i < 10; ++i) {
            buffer[i] = T{i};
            v.push_back(T{i});
            REQUIRE(equals_view(v, buffer_view<T>{buffer, (tm_size_t)i + 1}));
        }
        not_clobbered(v);
    }

    SUBCASE("swap1") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 2> a;
        small_vector<T, 5> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({T{1}, T{2}});                                                   // sbo
        b.assign({T{10}, T{9}, T{8}, T{7}, T{6}, T{5}, T{4}, T{3}, T{2}, T{1}});  // dynamic
        swap(a, b);
        REQUIRE(equals_list(a, {T{10}, T{9}, T{8}, T{7}, T{6}, T{5}, T{4}, T{3}, T{2}, T{1}}));
        REQUIRE(equals_list(b, {T{1}, T{2}}));
        swap(a, b);
        REQUIRE(equals_list(a, {T{1}, T{2}}));
        REQUIRE(equals_list(b, {T{10}, T{9}, T{8}, T{7}, T{6}, T{5}, T{4}, T{3}, T{2}, T{1}}));

        not_clobbered(a);
        not_clobbered(b);
    }

    SUBCASE("swap2") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 2> a;
        small_vector<T, 5> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({T{10}, T{9}, T{8}, T{7}, T{6}, T{5}, T{4}, T{3}, T{2}, T{1}});  // dynamic
        b.assign({T{1}, T{2}});                                                   // sbo
        swap(a, b);
        REQUIRE(equals_list(a, {T{1}, T{2}}));
        REQUIRE(equals_list(b, {T{10}, T{9}, T{8}, T{7}, T{6}, T{5}, T{4}, T{3}, T{2}, T{1}}));
        swap(a, b);
        REQUIRE(equals_list(b, {T{1}, T{2}}));
        REQUIRE(equals_list(a, {T{10}, T{9}, T{8}, T{7}, T{6}, T{5}, T{4}, T{3}, T{2}, T{1}}));

        not_clobbered(a);
        not_clobbered(b);
    }

    SUBCASE("swap3") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 2> a;
        small_vector<T, 5> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({T{1}, T{2}});                    // sbo
        b.assign({T{8}, T{7}, T{6}, T{5}, T{4}});  // sbo
        swap(a, b);
        REQUIRE(equals_list(a, {T{8}, T{7}, T{6}, T{5}, T{4}}));
        REQUIRE(equals_list(b, {T{1}, T{2}}));
        swap(a, b);
        REQUIRE(equals_list(b, {T{8}, T{7}, T{6}, T{5}, T{4}}));
        REQUIRE(equals_list(a, {T{1}, T{2}}));

        not_clobbered(a);
        not_clobbered(b);
    }

    SUBCASE("swap4") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> a;
        small_vector<T, 5> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({T{5}, T{4}, T{3}, T{2}, T{1}});  // sbo
        b.assign({T{1}, T{2}, T{3}, T{4}, T{5}});  // sbo
        swap(a, b);
        REQUIRE(equals_list(a, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(equals_list(b, {T{5}, T{4}, T{3}, T{2}, T{1}}));
        swap(a, b);
        REQUIRE(equals_list(b, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(equals_list(a, {T{5}, T{4}, T{3}, T{2}, T{1}}));

        not_clobbered(a);
        not_clobbered(b);
    }

    SUBCASE("swap5") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 1> a;
        small_vector<T, 1> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({T{5}, T{4}, T{3}, T{2}, T{1}});  // dynamic
        b.assign({T{1}, T{2}, T{3}, T{4}, T{5}});  // dynamic
        swap(a, b);
        REQUIRE(equals_list(a, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(equals_list(b, {T{5}, T{4}, T{3}, T{2}, T{1}}));
        swap(a, b);
        REQUIRE(equals_list(b, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(equals_list(a, {T{5}, T{4}, T{3}, T{2}, T{1}}));

        not_clobbered(a);
        not_clobbered(b);
    }

    SUBCASE("swap6") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 0> a;
        small_vector<T, 0> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({T{5}, T{4}, T{3}, T{2}, T{1}});  // dynamic
        b.assign({T{1}, T{2}, T{3}, T{4}, T{5}});  // dynamic
        swap(a, b);
        REQUIRE(equals_list(a, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(equals_list(b, {T{5}, T{4}, T{3}, T{2}, T{1}}));
        swap(a, b);
        REQUIRE(equals_list(b, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(equals_list(a, {T{5}, T{4}, T{3}, T{2}, T{1}}));

        not_clobbered(a);
        not_clobbered(b);
    }

    SUBCASE("swap7") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> a;
        small_vector<T, 5> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({});                              // sbo
        b.assign({T{1}, T{2}, T{3}, T{4}, T{5}});  // sbo
        swap(a, b);
        REQUIRE(equals_list(a, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(b.empty());
        swap(a, b);
        REQUIRE(a.empty());
        REQUIRE(equals_list(b, {T{1}, T{2}, T{3}, T{4}, T{5}}));

        not_clobbered(a);
        not_clobbered(b);
    }

    SUBCASE("swap8") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> a;
        small_vector<T, 5> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({});  // sbo
        b.assign({});  // sbo
        swap(a, b);
        REQUIRE(a.empty());
        REQUIRE(b.empty());
        swap(a, b);
        REQUIRE(a.empty());
        REQUIRE(b.empty());

        not_clobbered(a);
        not_clobbered(b);
    }

    SUBCASE("swap9") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 0> a;
        small_vector<T, 0> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({});  // sbo
        b.assign({});  // sbo
        swap(a, b);
        REQUIRE(a.empty());
        REQUIRE(b.empty());
        swap(a, b);
        REQUIRE(a.empty());
        REQUIRE(b.empty());

        not_clobbered(a);
        not_clobbered(b);
    }

    SUBCASE("copy assignment") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> a;
        small_vector<T, 5> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({});                              // sbo
        b.assign({T{1}, T{2}, T{3}, T{4}, T{5}});  // sbo
        a = b;
        REQUIRE(equals_list(a, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(equals_list(b, {T{1}, T{2}, T{3}, T{4}, T{5}}));

        not_clobbered(a);
        not_clobbered(b);
    }

    SUBCASE("move assignment") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> a;
        small_vector<T, 5> b;

        CAPTURE(a);
        CAPTURE(b);

        a.assign({});                              // sbo
        b.assign({T{1}, T{2}, T{3}, T{4}, T{5}});  // sbo
        a = std::move(b);
        REQUIRE(equals_list(a, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(b.empty());

        not_clobbered(a);
        not_clobbered(b);
    }

    SUBCASE("copy construct") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> a;
        a.assign({T{1}, T{2}, T{3}, T{4}, T{5}});  // sbo
        small_vector<T, 5> b(a);

        CAPTURE(a);
        CAPTURE(b);

        REQUIRE(equals_list(a, {T{1}, T{2}, T{3}, T{4}, T{5}}));
        REQUIRE(equals_list(b, {T{1}, T{2}, T{3}, T{4}, T{5}}));

        not_clobbered(a);
        not_clobbered(b);
    }

    SUBCASE("move construct") {
        allocation_guard sub_guard{};
        non_trivial_guard sub_non_trivial_destructors_guard{};
        small_vector<T, 5> a;
        a.assign({T{1}, T{2}, T{3}, T{4}, T{5}});  // sbo
        small_vector<T, 5> b(std::move(a));

        CAPTURE(a);
        CAPTURE(b);

        REQUIRE(a.empty());
        REQUIRE(equals_list(b, {T{1}, T{2}, T{3}, T{4}, T{5}}));

        not_clobbered(a);
        not_clobbered(b);
    }
}