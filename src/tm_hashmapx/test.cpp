// For printf to accept %llu on mingw
#define __USE_MINGW_ANSI_STDIO 1
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <algorithm>
#include <set>
#include <string>

#include "main.h"

#define TMH_INTERNAL_TESTING
#define TM_HASHMAPX_IMPLEMENTATION
#define TMH_X_NAME strmap
#define TMH_X_VALUE int
#define TMH_X_VALUE_IS_TRIVIAL
#define TMH_X_KEY_IS_STRING
#define TMH_X_IMPLEMENT
#include "main.h"
#undef TM_HASHMAPX_IMPLEMENTATION
#undef TMH_X_IMPLEMENT

#include "main.h"

#include "../common/tm_unreferenced_param.inc"

int main(int argc, char const *argv[]) {
    TM_UNREFERENCED_PARAM(argc);
    TM_UNREFERENCED_PARAM(argv);

    strmap s = strmap_create();
    if (!s.capacity)
    {
        printf("Out of memory\n");
        return -1;
    }

    struct Entry {
        const char* key;
        int value;
    };

    Entry entries[] = {
        {"asd", 0},
        {"asd1", 1},
        {"Hello", 2},
        {"asfhiaihf", 3},
        {"asfda3", 9},
        {"rzi0h3408hz", 1},
        {"asfapofdjdf9oh", 43},
        {"6c19453d-3ddf-4433-9600-df5e9c5ce7c2", 0},
        {"bbb07c98-3685-43d3-9489-fa8e2a7bfaf4", 1},
        {"594b0962-94cf-48c7-9116-492cb650d6c0", 2},
        {"c556bb7e-f8c7-477a-9ccc-da377cce6133", 3},
        {"92683580-9f54-4488-bf84-031d3be4d823", 4},
        {"41ece3b4-1c2c-4964-8c24-c47494fc84df", 5},
        {"9c253a8a-3652-4881-8208-ee44bfa44706", 6},
        {"665fca46-aad3-494e-ba38-aca493e89595", 7},
        {"d36f565f-97b1-4063-bf44-28b42be227ad", 8},
        {"09877a2e-c9cf-4c91-aad2-4784dd1ca492", 9},
        {"64e86554-07fd-4e4b-8ebc-302002138761", 10},
        {"3d531cd2-fd1b-4078-ab5f-2fb8e4e3f06c", 11},
        {"3ace9eed-43a9-4436-ac8f-4b9151f7e464", 12},
        {"50c3b8f9-e06d-4ce0-8b15-50dd5dd7ebe3", 13},
        {"ddad301a-9d42-4e9e-aac6-bf1e59b73a6f", 14},
        {"35f9ab8b-b5e1-4d74-b9eb-6c1b0685c62f", 15},
        {"009dc78f-d043-41a8-9af1-8dedeaf33abf", 16},
        {"b29c790b-21a3-4564-8b0f-dfdf0977055c", 17},
        {"8985033b-6f39-4b4a-913f-e52eacdb8597", 18},
        {"c6c0fcb0-f53a-43e5-bbd1-21f7cdc4f9ac", 19},
        {"d97ba73d-d119-4f3f-abd4-4f3223a8f22e", 20},
        {"c00eb4be-0d47-4033-8959-0fc8488117bc", 21},
        {"1d1b4f29-79fa-482d-81c3-7532fa7027fd", 22},
        {"b4c93c25-fd09-45b1-b28a-e416753a46a1", 23},
        {"bad02927-7b07-4da5-b100-b78bad5eb593", 24},
        {"7bfc57d9-8f31-425b-8364-af291f2f5fba", 25},
        {"7f363324-5c79-488b-a762-35626ebf83dc", 26},
        {"886493b8-8edd-4fde-9a8d-b59b21399437", 27},
        {"2f87d94a-b163-458e-bccf-a5d85b4165e7", 28},
        {"c123a413-c969-44c7-a777-794e2e9bba2e", 29},
        {"1b31ba20-206a-458f-91b1-df86887d2571", 30},
        {"deea1853-6e98-4a03-9d21-e98b2fb62b42", 31},
        {"ae108dc4-6508-456f-9183-163be46f5e81", 32},
        {"1596466c-73df-4a5e-8ea6-01a4d79d9aba", 33},
        {"0c19958c-68cd-4368-b536-dc14cfda0c77", 34},
        {"bd64f9e6-cd71-4f6d-8547-bb1629ca9465", 35},
        {"c4ad0bbe-4d7c-44af-ba6f-5b1b4ae0d92d", 36},
        {"7d4cd4ef-983c-4b8d-b866-b153780a0d5d", 37},
        {"2a27fc53-508e-4928-9d77-db7a1ce398b8", 38},
        {"4088ef6b-92f2-4b71-91cd-c413c3ddab09", 39},
        {"630c9cb1-9d0f-45d9-99e8-aaacb1826a56", 40},
        {"87749c2d-9506-4fe7-8956-a8621b15d38f", 41},
        {"c3c4cfab-d96f-42ba-adec-9a1d2c83e241", 42},
        {"b3bb8036-cba0-45ea-9add-960f9415f918", 43},
        {"34c83c5a-a15c-44aa-b6ef-3ab1d1107712", 44},
        {"fe62f6b0-e80f-4793-aa72-42dcbcc2709d", 45},
        {"b98cd366-204e-4c0f-801c-b2561a4b9744", 46},
        {"3bb890fd-e0fb-47d8-b593-c971e1f6c7cf", 47},
        {"2ff95ea7-a7b5-4941-8dbf-c06c5688ed45", 48},
        {"e966b201-31c5-400d-b251-4b37ad176ee5", 49},
        {"8c376493-52a6-4746-b2aa-8dc11718adea", 50},
        {"e2036117-e6d0-4685-bdb7-a7d2f8018c6a", 51},
        {"4ba08cad-5705-4948-a428-363c03e2515b", 52},
        {"eb0020ed-a66f-42ee-967c-cefa0dac69e2", 53},
        {"8fc607e7-9bd6-4cdf-bda8-cf94b15ed810", 54},
        {"2d6049fe-f0e3-4b1f-8ad1-0eaebad04286", 55},
        {"55449be7-d31e-4dd1-80bf-6a67a484c3b7", 56},
        {"f2ffbfef-df5b-4f57-8c4c-243bbb3df0fd", 57},
        {"8bb1a8ee-741c-4f41-9944-9c69419eaa1b", 58},
        {"bcd8e26b-cb6a-410b-b939-a5d4ea2c5920", 59},
        {"90269639-fed3-4545-8aa6-ba37e8495cf0", 60},
        {"0a78f25d-744f-4648-a7ee-868418732b64", 61},
        {"39851b0b-6217-4894-9f4f-3e6c1fc8dfbe", 62},
        {"c08f9bfa-d407-4ef4-a7d7-308b95bf734b", 63},
        {"fad52af0-5ca9-4122-b4c8-81d67e8efcf5", 64},
        {"8a438b6e-ea92-414c-b774-a01265b12707", 65},
        {"bde9c02b-7918-4fff-a909-c6f43cf74949", 66},
        {"7bd70288-7b0b-4892-8090-887f49245490", 67},
        {"45f9792e-941f-4ef2-aabf-532038cfcc0e", 68},
        {"fd012548-69ca-4495-b91c-901bd1006967", 69},
        {"908a8c9b-aae5-4fd3-b3df-c4e36f8deae8", 70},
        {"1196dd61-9e3b-4a9e-b955-67f5c8242ec4", 71},
        {"c63aee37-4183-4412-9743-71f9af21fdc3", 72},
    };

    for (auto&& entry : entries) {
        if (!strmap_insert(&s, entry.key, entry.value)) {
            printf("Insert of %s failed\n", entry.key);
            return -1;
        }
    }

    uint64_t count = 0;
    uint64_t probes = 0;
    uint64_t max_probe = 0;
    const char* max_probe_key = TM_NULL;
    for (auto&& entry : entries) {
        auto found = strmap_find(&s, entry.key);
        probes += strmap_probe_length;
        if (strmap_probe_length >= max_probe) {
            max_probe_key = entry.key;
            max_probe = strmap_probe_length;
        }
        ++count;
        if (!found) {
            printf("Couldn't find %s in map\n", entry.key);
            return -1;
        }
        printf("%s: %d, probes: %llu, (%s)\n", entry.key, *found, (unsigned long long)strmap_probe_length,
               (entry.value == *found) ? "SUCCESS" : "FAILED");
    }

#if 0
    strmap strmap_create();
    void strmap_destroy(strmap* map);
    tm_size_t strmap_size(strmap* map);
    tm_size_t strmap_capacity(strmap* map);
    tm_bool strmap_is_tombstone(strmap* map, tm_size_t index);
    tm_bool strmap_insert(strmap* map, const char* key, const int value);
    tm_bool strmap_insert_move(strmap* map, char** key, int* value);
    tm_bool strmap_remove(strmap* map, const char* key);
    tm_bool strmap_remove_index(strmap* map, tm_size_t index);
    int* strmap_find(strmap* map, const char* key);
    tm_bool strmap_find_index(strmap* map, const char* key, tm_size_t* index_out);
    int* strmap_get(strmap* map, tm_size_t index);
    char** strmap_get_key(strmap* map, tm_size_t index);
    #endif

    std::set<std::string> keys;
    for (tm_size_t i = 0; i < s.capacity; ++i) {
        if (strmap_is_tombstone(&s, i)) continue;
        auto pair = strmap_get(&s, i);
        if (!pair.key) {
            printf("No key for index %llu\n", (unsigned long long)i);
            return -1;
        }
        if (std::find_if(std::begin(entries), std::end(entries), [pair](const Entry& entry) {
                return strcmp(entry.key, *pair.key) == 0;
            }) == std::end(entries)) {
            printf("Key %s not found\n", *pair.key);
            return -1;
        }
        if (!keys.insert(*pair.key).second) {
            printf("Key %s is duplicate\n", *pair.key);
            return -1;
        }
    }

    printf("Map has %llu/%llu entries and is %f filled\n", (unsigned long long)s.count, (unsigned long long)s.capacity,
           (double)s.count / (double)s.capacity);
    printf("On average %f probes needed, with max %llu for %s\n", (double)probes / (double)count,
           (unsigned long long)max_probe, max_probe_key);
    strmap_destroy(&s);
    return 0;
}
