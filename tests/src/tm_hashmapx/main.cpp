#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <string>
#include <set>
#include <algorithm>

#include <assert_throws.h>
#include <assert_throws.cpp>

#include <redirected_malloc.cpp>

#define TM_HASHMAPX_IMPLEMENTATION
#define TMH_X_NAME cstrmap
#define TMH_X_VALUE int
#define TMH_X_VALUE_IS_TRIVIAL
#define TMH_X_KEY_IS_STRING
#define TMH_X_IMPLEMENT
#include <tm_hashmapx.h>
#undef TM_HASHMAPX_IMPLEMENTATION
#undef TMH_X_IMPLEMENT

size_t non_trivials_exist = 0;
struct non_trivial {
    int value[10];
    non_trivial() {
        set_value(0);
        ++non_trivials_exist;
    }
    non_trivial(int single_value) {
        set_value(single_value);
        ++non_trivials_exist;
    }
    non_trivial(non_trivial&& other) {
        set_value(other.value[0]);
        other.set_value(-1);
        ++non_trivials_exist;
    }
    non_trivial(const non_trivial& other) {
        set_value(other.value[0]);
        ++non_trivials_exist;
    }
    non_trivial& operator=(non_trivial&& other) {
        set_value(other.value[0]);
        other.set_value(-2);
        return *this;
    }
    non_trivial& operator=(const non_trivial& other) {
        set_value(other.value[0]);
        return *this;
    }
    ~non_trivial() { --non_trivials_exist; }

    void set_value(int i) {
        for (auto&& entry : value) entry = i;
    }
};

struct non_trivial_guard {
    size_t prev_non_trivials_exist;
    non_trivial_guard() : prev_non_trivials_exist(non_trivials_exist) {}
    ~non_trivial_guard() { CHECK(prev_non_trivials_exist == non_trivials_exist); }
};

// The library was not designed for C++ Classes, but we still test it.
// Do not take this as endorsement, that this is fine!
#define TMH_X_NAME stdstrmap

#define TMH_X_KEY std::string
#define TMH_X_KEY_INIT(key) (::new (static_cast<void*>(&(key))) std::string())
#define TMH_X_KEY_EQUALS(a, b) ((a) == (b))
#define TMH_X_KEY_COPY(dest, src) (((dest) = (src)), TM_TRUE)
#define TMH_X_KEY_MOVE(dest, src) (::new (static_cast<void*>(&(dest))) std::string(std::move(*(src))))
#define TMH_X_KEY_DESTROY(key) (key).~basic_string()
#define TMH_X_KEY_IS_TOMBSTONE(key) ((key).empty())
#define TMH_X_KEY_SET_TOMBSTONE(key) ((key).clear())
#define TMH_X_KEY_HASH(key) tmh_string_hash((key).c_str())

#define TMH_X_VALUE non_trivial
#define TMH_X_VALUE_INIT(value) (::new (static_cast<void*>(&(value))) non_trivial())
#define TMH_X_VALUE_COPY(dest, src) (((dest) = (src)), TM_TRUE)
#define TMH_X_VALUE_MOVE(dest, src) (::new (static_cast<void*>(&(dest))) non_trivial(std::move(*(src))))
#define TMH_X_VALUE_DESTROY(value) (value).~non_trivial()

#define TMH_X_IMPLEMENT
#include <tm_hashmapx.h>
#undef TMH_X_IMPLEMENT

// Simple Wrappers
struct MapPair {
    bool is_tombstone;
    std::string key;
    int value;
};

struct StrMap {
    cstrmap map;

    StrMap() { map = cstrmap_create(); }
    ~StrMap() { cstrmap_destroy(&map); }

    tm_size_t count() const { return cstrmap_count(&map); }
    tm_size_t capacity() const { return cstrmap_capacity(&map); }

    tm_bool insert(const char* key, int value) { return cstrmap_insert(&map, key, value); }
    tm_bool remove(const char* key) { return cstrmap_remove(&map, key); }
    int* find(const char* key) { return cstrmap_find(&map, key); }
    MapPair get(tm_size_t index) {
        MapPair result;
        auto pair = cstrmap_get(&map, index);
        if (pair.key) {
            REQUIRE(pair.value);
            result.is_tombstone = false;
            result.key = *pair.key;
            result.value = *pair.value;
        } else {
            result.is_tombstone = true;
            result.value = -10;
        }
        return result;
    }
};

struct StdStrMap {
    stdstrmap map;

    StdStrMap() { map = stdstrmap_create(); }
    ~StdStrMap() { stdstrmap_destroy(&map); }

    tm_size_t count() const { return stdstrmap_count(&map); }
    tm_size_t capacity() const { return stdstrmap_capacity(&map); }

    tm_bool insert(const char* key, int value) { return stdstrmap_insert(&map, key, value); }
    tm_bool remove(const char* key) { return stdstrmap_remove(&map, key); }
    int* find(const std::string& key) {
        auto entry = stdstrmap_find(&map, key);
        if (entry) return &entry->value[0];
        return nullptr;
    }
    MapPair get(tm_size_t index) {
        MapPair result;
        auto pair = stdstrmap_get(&map, index);
        if (pair.key) {
            REQUIRE(pair.value);
            result.is_tombstone = false;
            result.key = *pair.key;
            result.value = pair.value->value[0];
        } else {
            result.is_tombstone = true;
            result.value = -10;
        }
        return result;
    }
};

TEST_CASE_TEMPLATE("insert", T, StrMap, StdStrMap) {
    allocation_guard guard{};
    non_trivial_guard non_trivial_destructors_guard{};

    struct Entry {
        const char* key;
        int value;
    };

    const Entry entries[] = {
        {"57e3c516-1d7d-46b5-b0c9-5084664b54f5", 0},   {"22894156-1749-4499-8c63-fc367b94f1e5", 1},
        {"9911f05c-3dfb-406d-94bc-f9e182926e70", 2},   {"bb36c9b1-44c6-48d5-9d58-3512eb8ce0e3", 3},
        {"d2e8b0d4-8474-428e-b692-24ae7884ca6a", 4},   {"3c1ad476-24a3-48e6-a920-4f57324d708e", 5},
        {"3f860561-6759-47cc-9f5a-e475ec350a87", 6},   {"cb46055b-577e-47e3-956a-cb43e8f9cbba", 7},
        {"c4b0cda0-9d1c-4c49-a2a3-dae5683b870d", 8},   {"7ca9e91e-78e1-47db-8cdd-84b74dca38bf", 9},
        {"373f8a23-34bb-497f-92d5-cebf22fefa0a", 10},  {"3a7e2f6e-090b-4b1b-909f-a488c91209d1", 11},
        {"57319965-eef2-4122-b411-f7624d39acb6", 12},  {"94d5cb6d-5f58-4807-81f6-f492b3a20d47", 13},
        {"619c6a92-0c58-4b52-8889-df26c0e3e3c4", 14},  {"2510223a-bb07-4fb0-abfe-ddb20973abcd", 15},
        {"c35080fc-f234-4efd-81e6-690f340afa13", 16},  {"3d928f0c-8cc7-4f3c-958a-63a1080dab63", 17},
        {"c1e54108-ec5f-4a00-b527-f76cbe2e4deb", 18},  {"ac7294fe-9ec9-4d6c-b9de-955284327f91", 19},
        {"152130cb-fade-4bbe-9258-04b26743f78a", 20},  {"1232e5af-ff06-4094-aeca-347ae5b4f605", 21},
        {"839c56ed-1079-41bd-bc84-603973e217ac", 22},  {"7743e092-da94-4ccd-8453-0f275e9190e8", 23},
        {"c0a7e285-907c-465b-a78f-39ec3382814d", 24},  {"6f8629e7-9452-46e9-9581-8b096657ee4a", 25},
        {"3ecffed9-c2ce-4b72-ad55-cbee152c3e66", 26},  {"8f7880f4-9377-46b4-b52f-a2b9a74607be", 27},
        {"eed2f8ba-5546-4a19-9e0d-ab69a99d0b83", 28},  {"a47cf34b-16dc-45a3-8aa2-8e26329a7b73", 29},
        {"4f31f26b-0efd-4989-ad08-c09b19ec7c90", 30},  {"e302465e-a010-479e-b17f-d99945e4be79", 31},
        {"67fc56b0-936a-4154-bcf9-6d1114cceb38", 32},  {"0d4bb7cf-cc4c-4c96-9dfd-72a59bda395f", 33},
        {"9ffd3884-bdca-41bd-ac0d-a848c38f1621", 34},  {"21487320-3d86-4efc-baba-447137d8e3b2", 35},
        {"c7fdec62-a184-47a2-baa7-5bc7ef3855d0", 36},  {"789d7b2c-5fcd-499e-830c-4d7c6696deca", 37},
        {"b86a9402-5ffb-40d5-9889-5a4f82e3991b", 38},  {"22babd74-bf2a-4fe1-9d92-6e95f3491bb0", 39},
        {"71cca049-42bf-4d46-aa9b-170b05cb08cf", 40},  {"3e30c40c-0592-45aa-b90a-8ab8da2718fb", 41},
        {"b69a0b1b-5c4a-4d19-8045-2f6615fa4280", 42},  {"ad7ae150-adbc-465c-97c1-fdaed84bccf6", 43},
        {"d9eda000-8ca5-4392-b0b0-11a6b465fc43", 44},  {"fcc75848-ac90-4be9-9019-90cc7807af7f", 45},
        {"8b120348-97fe-45c4-8ef3-b4c9956eecaf", 46},  {"faaaffe9-08e7-4834-a995-f71728489cc4", 47},
        {"d4b097b1-931c-4772-91f5-dc502c8310df", 48},  {"10d3b327-7f4f-4dbb-8bcd-c10dbe8e6d60", 49},
        {"4b869394-d8c2-4a86-bcb9-76c2fb20e650", 50},  {"9d261500-75f7-4c69-a186-3a1d381a0d64", 51},
        {"f94f06da-a5c0-4a6a-8c12-3c57f54dc280", 52},  {"cf4eebbf-b0ec-4bba-9e64-fd4dfa7598d7", 53},
        {"617ddfc4-f380-42d9-ab35-f4dc23b11ed2", 54},  {"639a1c54-f1b3-47fd-88f3-e03b1aaac533", 55},
        {"9bf250dd-872b-4350-a5b7-d0b847fe81b5", 56},  {"5ef70c36-947a-49e9-a5ea-1470c5352bc0", 57},
        {"15018136-7cb6-492b-9fb7-a2bdda23fa8e", 58},  {"b3cc1c76-4c65-438e-998c-a63bc25e5c67", 59},
        {"92b7a008-da1b-4ed7-8c1e-6f3101010e92", 60},  {"ce12cd0e-c0ce-4f82-a0dd-998931685a72", 61},
        {"828a3958-4865-4295-b37e-b77af4ca7e6a", 62},  {"a23a7ba5-5020-47b1-a88c-00e4a99e9f8b", 63},
        {"efe54f0c-6d45-46a9-aeb8-857deb15d495", 64},  {"fa6e5a97-0038-4870-88f2-29c063c7fece", 65},
        {"d32ff8b2-0929-42d2-bbbb-e27210c4e734", 66},  {"dd73ffb3-c47f-4468-8138-d2b1a1400852", 67},
        {"48836b9a-9803-4493-94cc-63c777b5f7f5", 68},  {"8b8eefab-db40-4331-9176-bccd6e923e8a", 69},
        {"53be893f-ab56-4700-a5bf-8c93464fc320", 70},  {"06917876-652e-4842-8640-778a35489566", 71},
        {"b5f778ee-6c68-43c1-9470-f1ffb6a17b34", 72},  {"ce0bae53-4f70-4bb1-bfa0-98a5b5424a6e", 73},
        {"288e619b-b3c1-4617-9170-f70e7e0f2cf9", 74},  {"e147333b-9043-4ff6-8772-0ef00daa8b86", 75},
        {"e1bcdbf8-efbf-4391-82f7-6ec101d4b8b7", 76},  {"a6444308-3227-4311-b581-540cb4be6c54", 77},
        {"ccae8d8d-6e3a-4728-8e4b-101491c9e0f0", 78},  {"e95ae2b7-dbe2-4df8-8f7a-1311733966ad", 79},
        {"f65c7a86-e54b-49d1-bc84-c20497da2d2f", 80},  {"e2ab19e8-f0c9-4ead-940b-a58dac03abe4", 81},
        {"247c81ac-3568-4c6a-bad5-48f93a6cabc3", 82},  {"39d86cf7-a7d8-437d-b617-1d97a25a5810", 83},
        {"f335ce90-045c-4dcc-93d2-0ee81f65672c", 84},  {"743aecf9-7821-4569-953d-16822603d155", 85},
        {"b0418165-ce02-49ac-977e-0ad6f9b065b9", 86},  {"ab623a1f-6fb0-4546-b940-82446d3e8869", 87},
        {"ad9195a9-cf50-4b74-8fbc-1bd49b901d4c", 88},  {"c8f6b41d-2592-475f-b7b2-e3816afaaa48", 89},
        {"9782ca36-22cf-4688-a075-a6cc76e2d2d1", 90},  {"70f6e1c0-03a9-4abd-a2a9-b7259fa2950b", 91},
        {"33f03e03-2b63-43a0-9c34-8e49c1f761a8", 92},  {"a6bdf552-264c-466e-808e-eb18b984b721", 93},
        {"c7c45fa3-ef4d-4763-86f0-993217e02f14", 94},  {"bd70e3d3-cc59-4f08-8d41-5014f405ad9a", 95},
        {"872238d8-b2a5-4f1b-93b2-509cba08ee28", 96},  {"cdfb36ea-8436-4a7a-891e-440361d11d23", 97},
        {"59ee65e7-f206-49b5-82bb-2fe565d7d6f3", 98},  {"2ebc542d-8b4d-45ae-9b66-95ab9000642b", 99},
        {"c6d4c3e4-a514-4b56-8db2-0b3fb6b063d7", 100},
    };

    T map;

    for (auto&& entry : entries) {
        REQUIRE(map.insert(entry.key, entry.value));
        auto found = map.find(entry.key);
        REQUIRE(found);
        REQUIRE(*found == entry.value);
    }
    REQUIRE(map.map.count == (tm_size_t)std::size(entries));

    std::set<std::string> found_entries;
    for (tm_size_t i = 0, count = map.map.count; i < count; ++i) {
        auto entry = map.get(i);
        if (entry.is_tombstone) continue;
        REQUIRE(found_entries.insert(entry.key).second);
        auto existing_entry = std::find_if(std::begin(entries), std::end(entries), [&](const Entry& existing) {
            return std::string(existing.key) == entry.key;
        });
        REQUIRE(existing_entry != std::end(entries));
        REQUIRE(existing_entry->value == entry.value);
    }

    for (auto&& entry : entries) {
        REQUIRE(map.remove(entry.key));
        REQUIRE(!map.find(entry.key));
        REQUIRE(!map.remove(entry.key));
    }
    REQUIRE(map.map.count == 0);
}

TEST_CASE_TEMPLATE("multi insert", T, StrMap, StdStrMap) {
    T map;
    REQUIRE(map.insert("test", 1));
    REQUIRE(map.insert("test", 2));
    REQUIRE(map.insert("test", 3));
    REQUIRE(map.insert("test", 4));
    REQUIRE(map.insert("test", 5));
    REQUIRE(map.insert("test", 6));
    REQUIRE(map.insert("test", 7));
    REQUIRE(map.map.count == 1);
    auto entry = map.find("test");
    REQUIRE(entry);
    REQUIRE(*entry == 7);
}