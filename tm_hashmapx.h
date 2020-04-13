/*
tm_hashmapx.h v0.0.1 - public domain - https://github.com/to-miz/tm
Author: Tolga Mizrak 2020

No warranty; use at your own risk.

LICENSE
    See license notes at end of file.

DESCRIPTION
    A hashmap implementation library using X-Macros.
    See https://en.wikipedia.org/wiki/X_Macro

    The properties of the hashmap are as follows:
        - power-of-two sized for fast range reduction.
        - Hash conflicts are resolved with linear-probing
        - The max linear-probing threshold is 16 (configurable).
        - The occupancy threshold for growth/rehashing is 75%.

    The default hashing algorithm implemented in this library is fnv1a.

    Note that this library is primarily targeted at C and not C++.
    Do not use it with non-pod types.

USAGE
    This file works as both the header and implementation.
    To implement the interfaces in this header,
        #define TM_HASHMAPX_IMPLEMENTATION
    in ONE C source file before #including this header.

    This header is also used for X-Macro instantiation. If you define these macros before
    including this header, a typesafe hashmap-type will be generated. See the following example:

        // File: strmap.h
        #define TMH_X_NAME strmap
        #define TMH_X_VALUE int
        #define TMH_X_VALUE_IS_TRIVIAL
        #define TMH_X_KEY_IS_STRING
        #include <tm/tm_hashmapx.h>

    This will generate the following interfaces:

        typedef struct {
            char** keys;
            int* values;
            tm_size_t count;
            tm_size_t capacity;
        } strmap;

        strmap strmap_create();
        void strmap_destroy(strmap* map);
        tm_size_t strmap_count(strmap* map);
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

    For the implementation of the interfaces it is then necessary to do the following
    in a source file:

        // File: strmap.c
        #define TMH_X_IMPLEMENT
        #include "strmap.h"

    See SWITCHES for an indepth explanation of the X-Macros.
    The documentation for these functions can be found at the declaration site.

SWITCHES
    TMH_X_NAME:
        The name of the struct defining the hashmap. Example:
            #define TMH_X_NAME my_hashmap
        will result in:
            typedef struct {
                ...
            } my_hashmap;

    TMH_X_KEY_IS_STRING:
        Convenience macro for the case when the key type is char*.
        Will define the following macros automatically as follows:
            #define TMH_X_KEY                    char*
            #define TMH_X_KEY_EQUALS(a, b)       (tmh_string_equals((a), (b)))
            #define TMH_X_KEY_COPY(dest, src)    (tmh_string_copy(&(dest), src))
            #define TMH_X_KEY_MOVE(dest, src)    tmh_string_destructive_move(&(dest), (src))
            #define TMH_X_KEY_DESTROY(key)       tmh_string_destroy(&(key))
            #define TMH_X_KEY_HASH(key)          tmh_string_hash((key))
            #define TMH_X_KEY_SET_TOMBSTONE(key) (key = TM_NULL)
            #define TMH_X_KEY_IS_TOMBSTONE(key)  (key == TM_NULL)

    TMH_X_VALUE_IS_TRIVIAL:
        Convenience macro for the case when the value type is trivial
        and no special handling for copying/moving is needed.
        Will define the following macros automatically as follows:
        #define TMH_X_VALUE_COPY(dest, src) (((dest) = (src)), TM_TRUE)
        #define TMH_X_VALUE_MOVE(dest, src) ((dest) = *(src))
        #define TMH_X_VALUE_INIT(dest)      (TM_MEMSET(&(dest), 0, sizeof(dest)))
        #define TMH_X_VALUE_DESTROY(value)  ((void)0)

    TMH_X_KEY:
        The key type. All TMH_X_KEY_ prefixed macros expect parameters of this type. Example:
            #define TMH_X_KEY char*

    TMH_X_KEY_INIT: (optional)
        The initializer of the key, in case it is non trivial. Example:
            #define TMH_X_KEY_INIT(key) (key = malloc(...))

    TMH_X_KEY_EQUALS(existing_key, search_key):
        Checks the equality of two keys. Should return TM_TRUE if they are equal and TM_FALSE otherwise.
        It is safe to take the address of both parameters.

    TMH_X_KEY_COPY(dest, src):
        Creates a new copy of a key. Should return TM_TRUE on success and TM_FALSE otherwise.
        It is safe to take the address of both parameters. Example:
            #define TMH_X_KEY char*
            #define TMH_X_KEY_COPY(dest, src)    (tmh_string_copy(&(dest), src))

    TMH_X_KEY_HASH(key):
        Returns the hash of a given key as uint64_t.

    TMH_X_KEY_SET_TOMBSTONE(dest_key):
        Sets the destination key to the tombstone key value. For pointer types usually NULL.
        It is safe to take the address of dest_key.

    TMH_X_KEY_IS_TOMBSTONE(key):
        Should return TM_TRUE if key is the tombstone value, TM_FALSE otherwise.

    TMH_X_KEY_DESTROY(key):
        Destroys/deallocates a given key.
        It is safe to take the address of key.
            #define TMH_X_KEY_DESTROY(key)       tmh_string_destroy(&(key))

    TMH_X_KEY_MOVE(dest, src):
        Performs a destructive move from src to dest. TMH_X_KEY_INIT will not be called on dest beforehand.
        Usually corresponds to a memcpy of src to dest, followed by a memset of src to 0, so that the
        resources are not freed my TMH_X_KEY_DESTROY.
        Corresponds to a "move constructor" in C++ terms.
        It is safe to take the address of both parameters.

    TMH_X_VALUE:
        The value type. All TMH_X_VALUE_ prefixed macros expect parameters of this type. Example
            #define TMH_X_VALUE int

    TMH_X_VALUE_INIT(value):
        The initializer of a given value. Example:
            #define TMH_X_VALUE_INIT(value) ((value) = 0)

    TMH_X_VALUE_COPY(dest, src):
        Creates a new copy of a value. Should return TM_TRUE on success and TM_FALSE otherwise.
        It is safe to take the address of both parameters. Example:
            #define TMH_X_VALUE char*
            #define TMH_X_VALUE_COPY(dest, src)    (tmh_string_copy(&(dest), src))

    TMH_X_VALUE_MOVE(dest, src): (optional)
        Moves src into dest. The value at dest will not have been "constructed" yet,
        so TMH_X_VALUE_INIT wasn't called on it.
        Corresponds to a "move constructor" in C++ terms.

    TMH_X_VALUE_DESTROY(value):
        Destroys/deallocates a given value.
        It is safe to take the address of value. Example:
            #define TMH_X_VALUE_DESTROY(value)     tmh_string_destroy(&(value))

    TMH_X_FUNCPREFIX:
        The prefix of the generated functions. Default: TMH_X_NAME

    TMH_CUSTOM_SUFFIXES:
        Define this to override the suffixes of the generated functions/structs. The macros for those are:
        TMH_CREATE:       Default: _create
        TMH_DESTROY:      Default: _destroy
        TMH_INSERT:       Default: _insert
        TMH_INSERT_MOVE:  Default: _insert_move
        TMH_FIND:         Default: _find
        TMH_GET:          Default: _get
        TMH_REMOVE:       Default: _remove
        TMH_REMOVE_INDEX: Default: _remove_index
        TMH_COUNT:        Default: _count
        TMH_CAPACITY:     Default: _capacity
        TMH_IS_TOMBSTONE: Default: _is_tombstone
        TMH_REHASH:       Default: _rehash
        TMH_GROW:         Default: _grow
        TMH_FIND_INDEX:   Default: _find_index
        TMH_PAIR:         DEFAULT: _pair

HISTORY
    v0.0.1  10.04.20 Initial commit.
*/

/* This is a generated file, do not modify directly. You can find the generator files in the src directory. */

// clang-format off

/* Dependencies */
#if defined(TM_HASHMAPX_IMPLEMENTATION) || defined(TMH_X_IMPLEMENT)
    /* Global allocation functions to use. */
    #if !defined(TM_MALLOC) || !defined(TM_REALLOC) || !defined(TM_FREE)
        // Either all or none have to be defined.
        #include <stdlib.h>
        #define TM_MALLOC(size, alignment) malloc((size))
        #define TM_REALLOC(ptr, old_size, old_alignment, new_size, new_alignment) realloc((ptr), (new_size))
        // #define TM_REALLOC_IN_PLACE(ptr, old_size, old_alignment, new_size, new_alignment) // Optional
        #define TM_FREE(ptr, size, alignment) free((ptr))
    #endif

    /* assert */
    #ifndef TM_ASSERT
        #include <assert.h>
        #define TM_ASSERT assert
    #endif /* !defined(TM_ASSERT) */

    #if !defined(TM_MEMSET) || !defined(TM_STRLEN) \
        || !defined(TM_MEMCPY) || !defined(TM_STRCMP)

        #include <string.h>
        #ifndef TM_MEMSET
            #define TM_MEMSET memset
        #endif
        #ifndef TM_STRLEN
            #define TM_STRLEN strlen
        #endif
        #ifndef TM_MEMCPY
            #define TM_MEMCPY memcpy
        #endif
        #ifndef TM_STRCMP
            #define TM_STRCMP strcmp
        #endif
    #endif
#endif /* defined(TM_HASHMAPX_IMPLEMENTATION) || defined(TMH_X_IMPLEMENT) */

#ifndef TM_HASHMAPX_H_INCLUDED_81E2D72E_A4AC_440F_A32D_A68D64020DC3_
#define TM_HASHMAPX_H_INCLUDED_81E2D72E_A4AC_440F_A32D_A68D64020DC3_

/* Fixed width ints. Include C version so identifiers are in global namespace. */
#include <stdint.h>

/* size_t is unsigned by default, but we also allow for signed and/or 32bit size_t.
   You can override this block by defining TM_SIZE_T_DEFINED and the typedefs before including this file. */
#ifndef TM_SIZE_T_DEFINED
    #define TM_SIZE_T_DEFINED
    #define TM_SIZE_T_IS_SIGNED 0 /* Define to 1 if tm_size_t is signed. */
    #include <stddef.h> /* Include C version so identifiers are in global namespace. */
    typedef size_t tm_size_t;
#endif /* !defined(TM_SIZE_T_DEFINED) */

/* Native bools, override by defining TM_BOOL_DEFINED yourself before including this file. */
#ifndef TM_BOOL_DEFINED
    #define TM_BOOL_DEFINED
    #ifdef __cplusplus
        typedef bool tm_bool;
        #define TM_TRUE true
        #define TM_FALSE false
    #else
        typedef _Bool tm_bool;
        #define TM_TRUE 1
        #define TM_FALSE 0
    #endif
#endif /* !defined(TM_BOOL_DEFINED) */

/* Use null of the underlying language. */
#ifndef TM_NULL
    #ifdef __cplusplus
        #define TM_NULL nullptr
    #else
        #define TM_NULL NULL
    #endif
#endif

#ifndef TMH_DEF
    #define TMH_DEF extern
#endif

#ifndef TMH_MAX_LINEAR_PROBE
    #define TMH_MAX_LINEAR_PROBE 16
#endif

#ifndef TMH_CUSTOM_SUFFIXES
    // Suffixes to the function names,
    // #define TMH_CUSTOM_SUFFIXES yourself to change them.

    #define TMH_CREATE _create
    #define TMH_DESTROY _destroy
    #define TMH_INSERT _insert
    #define TMH_INSERT_MOVE _insert_move
    #define TMH_FIND _find
    #define TMH_GET _get
    #define TMH_REMOVE _remove
    #define TMH_REMOVE_INDEX _remove_index
    #define TMH_COUNT _count
    #define TMH_CAPACITY _capacity
    #define TMH_IS_TOMBSTONE _is_tombstone
    #define TMH_REHASH _rehash
    #define TMH_GROW _grow
    #define TMH_FIND_INDEX _find_index
    #define TMH_PAIR _pair

    #define TMH_CUSTOM_SUFFIXES
#endif

#ifndef TMH_STR_JOIN
    #define TMH_STR_JOIN2(a, b) a##b
    #define TMH_STR_JOIN(a, b) TMH_STR_JOIN2(a, b)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Hashes a memory region byte by byte using the fnv1a algorithm.
 * @param data[IN] The input data to be hashed. Can be @c NULL if size is 0.
 * @param size[IN] The size in bytes of data. Can be 0.
 * @return The calculated hash.
 */
TMH_DEF uint64_t tmh_hash(const void* data, tm_size_t size);

/*!
 * @brief Hashes a memory region byte by byte using the fnv1a algorithm
 * and combines it with a precalculated hash. This allows for chaining together tmh_hash
 * and tmh_hash_combine to hash complex objects.
 * @param data[IN] The input data to be hashed. Can be @c NULL if size is 0.
 * @param size[IN] The size in bytes of data. Can be 0.
 * @param prev_hash[IN] The previously calculated hash. Should not be 0. Use tmh_hash or tmh_string_hash
 * to calculate the initial hash.
 * @return The new calculated hash, which can be passed into another call to tmh_hash_combine.
 */
TMH_DEF uint64_t tmh_hash_combine(const void* data, tm_size_t size, uint64_t prev_hash);

/*!
 * @brief Hashes a nullterminated string using the fnv1a algorithm.
 * @param str[IN] The input string. Can be empty, but must not be @c NULL.
 * @return The calculated hash.
 */
TMH_DEF uint64_t tmh_string_hash(const char* str);

/*!
 * @brief Hashes a nullterminated string using the fnv1a algorithm.
 * and combines it with a precalculated hash. This allows for chaining together tmh_string_hash
 * and tmh_string_hash_combine to hash multiple strings into a single hash.
 * @param str[IN] The input string. Can be empty, but must not be @c NULL.
 * @param prev_hash[IN] The previously calculated hash. Should not be 0. Use tmh_hash or tmh_string_hash
 * to calculate the initial hash.
 * @return The calculated hash.
 */
TMH_DEF uint64_t tmh_string_hash_combine(const char* str, uint64_t prev_hash);

/*!
 * @brief Helper function, which returns TM_TRUE of @param x is a power of two.
 * @return @c TM_TRUE if x is a power of two, @c TM_FALSE otherwise.
 */
TMH_DEF tm_bool tmh_is_power_of_two(uint64_t x);

// Some helpers for string keys/values.

/*!
 * @brief Copies a nullterminated string into @param out using malloc.
 * @param out[OUT] The output parameter. *out should not have valid data beforehand.
 * @param src[IN] The input string. Can be NULL, in which case nothing will be allocated,
 * but the result will still be @c TM_TRUE.
 * @return @c TM_TRUE if allocation was successful, @c TM_FALSE otherwise.
 */
TMH_DEF tm_bool tmh_string_copy(char** out, const char* src);

/*!
 * @brief Compares two nullterminated strings for equality.
 * @param lhs[IN] The first string. Can be @c NULL .
 * @param rhs[IN] The second string. Can be @c NULL .
 * @return @c TM_TRUE if the strings are equal, @c TM_FALSE otherwise.
 */
TMH_DEF tm_bool tmh_string_equals(const char* lhs, const char* rhs);

/*!
 * @brief Deallocates a given nullterminated string.
 * @param str[IN] A pointer to a string allocated using tmh_string_copy.
 * Can be @c NULL and the pointed to value can also be @c NULL.
 * The pointed to value will be set to @c NULL after freeing.
 */
TMH_DEF void tmh_string_destroy(char** str);

/*!
 * @brief Performs a destructive move from @param src to @param dest.
 * @param dest[OUT] The output parameter. *out should not have valid data beforehand.
 * @param src[IN,OUT] The input parameter, *src will be set to @c NULL afterwards.
 */
TMH_DEF void tmh_string_destructive_move(char** dest, char** src);

#ifdef __cplusplus
}
#endif

#endif /* defined(TM_HASHMAPX_H_INCLUDED_81E2D72E_A4AC_440F_A32D_A68D64020DC3_) */

#ifdef TMH_X_NAME

#ifndef TMH_X_FUNCPREFIX
    #define TMH_X_FUNCPREFIX TMH_X_NAME
#endif

#ifdef TMH_X_VALUE_IS_TRIVIAL
    #define TMH_X_VALUE_COPY(dest, src) (((dest) = (src)), TM_TRUE)
    #define TMH_X_VALUE_MOVE(dest, src) ((dest) = *(src))
    #define TMH_X_VALUE_INIT(dest) (TM_MEMSET(&(dest), 0, sizeof(dest)))
    #define TMH_X_VALUE_DESTROY(value) ((void)0)
#else
    #ifndef TMH_X_VALUE_MOVE
        #define TMH_X_VALUE_MOVE(dest, src) ((dest) = *(src))
    #endif
    #ifndef TMH_X_VALUE_COPY
        #define TMH_X_VALUE_COPY(dest, src) (((dest) = (src)), TM_TRUE)
    #endif
#endif

#ifdef TMH_X_KEY_IS_STRING
    #define TMH_X_KEY char*
    #define TMH_X_KEY_EQUALS(a, b) (tmh_string_equals((a), (b)))
    #define TMH_X_KEY_COPY(dest, src) (tmh_string_copy(&(dest), src))
    #define TMH_X_KEY_MOVE(dest, src) tmh_string_destructive_move(&(dest), (src))
    #define TMH_X_KEY_DESTROY(key) tmh_string_destroy(&(key))
    #define TMH_X_KEY_HASH(key) tmh_string_hash((key))
    #define TMH_X_KEY_IS_TOMBSTONE(key) (key == TM_NULL)
    #define TMH_X_KEY_SET_TOMBSTONE(key) (key = TM_NULL)
#endif

#ifndef TMH_X_KEY_MOVE
    #define TMH_X_KEY_MOVE(dest, src) ((dest) = *(src))
#endif
#ifndef TMH_X_KEY_INIT
    #define TMH_X_KEY_INIT(key) ((void)0)
#endif

// clang-format on

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief A hashmap using linear-probing.
 */
typedef struct {
    TMH_X_KEY* keys;
    TMH_X_VALUE* values;
    tm_size_t count;
    tm_size_t capacity;
} TMH_X_NAME;

/*!
 * @brief A structure for the key/value pair.
 */
typedef struct {
    TMH_X_KEY const* key;
    TMH_X_VALUE* value;
} TMH_STR_JOIN(TMH_X_NAME, TMH_PAIR);

/*!
 * @brief Creates a new hashmap with a certain minimum capacity.
 * @return The created hashmap. If the creation failed, the capacity field will be 0.
 */
TMH_DEF TMH_X_NAME TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_CREATE)();

/*!
 * @brief Destroys a hashmap, freeing all resources and memory.
 * @param map[IN] The hashmap to be destroyed. Can be @c NULL . Afterwards all fields will be @c NULL or 0.
 */
TMH_DEF void TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_DESTROY)(TMH_X_NAME* map);

/*!
 * @brief Inserts a key/value pair into the hashmap. If the key already exists, its value will be overwritten.
 * @param map[IN,OUT] The hashmap. Must not be @c NULL .
 * @param key[IN] The key to be inserted. Should not be the tombstone value, otherwise this function fails.
 * @param value[IN] The value to be inserted.
 * @return @c TM_TRUE if insertion was successful, @c TM_FALSE otherwise (most likely because allocation failed).
 */
TMH_DEF tm_bool TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_INSERT)(TMH_X_NAME* map, const TMH_X_KEY key,
                                                           const TMH_X_VALUE value);

/*!
 * @brief Inserts a key/value pair into the hashmap by doing a destructive move of the parameters.
 * The parameters should not be freed afterwards if this function returns @c TM_TRUE .
 * Used primarily for more efficient rehashing.
 * @param map[IN,OUT] The hashmap. Must not be @c NULL .
 * @param key[IN,OUT] The key to be moved while inserting. Should not be the tombstone value, otherwise this function
 * fails. Will be overwritten on success.
 * @param value[IN,OUT] The value to be moved while inserting. Will be overwritten on success.
 * @return @c TM_TRUE if insertion was successful, @c TM_FALSE otherwise (most likely because allocation failed).
 */
TMH_DEF tm_bool TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_INSERT_MOVE)(TMH_X_NAME* map, TMH_X_KEY* key, TMH_X_VALUE* value);

/*!
 * @brief Finds the value to a given key.
 * @param map[IN] The const hashmap. Must not be @c NULL .
 * @param key[IN] The const key.
 * @return A pointer to the value if the key/value pair exists, @c NULL otherwise.
 */
TMH_DEF TMH_X_VALUE* TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_FIND)(const TMH_X_NAME* map, const TMH_X_KEY key);

/*!
 * @brief Finds the index of a given key/value pair.
 * @param map[IN] The const hashmap. Must not be @c NULL .
 * @param key[IN] The const key.
 * @param index_out[OUT] The output index. Must not be @c NULL . Will only be written to on success.
 * @return @c TM_TRUE if the key was found, @c TM_FALSE otherwise.
 */
TMH_DEF tm_bool TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_FIND_INDEX)(const TMH_X_NAME* map, const TMH_X_KEY key,
                                                               tm_size_t* index_out);

/*!
 * @brief Returns the key/value pair to a given index. Can be used to iterate through all pairs of a hashmap.
 * Note that you need to iterate using map->capacity and not count.
 * @param map[IN] The const hashmap. Must not be @c NULL .
 * @param index[IN] The index of the requested key/value pair. Should be 0 <= index < map->capacity.
 * @return A structure with pointers to the key and value. The pointers will be @c NULL if the index points to a
 * tombstone.
 */
TMH_DEF TMH_STR_JOIN(TMH_X_NAME, TMH_PAIR)
    TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_GET)(const TMH_X_NAME* map, tm_size_t index);

/*!
 * @brief Removes a key/value pair from the hashmap.
 * @param map[IN,OUT] The hashmap. Must not be @c NULL .
 * @param key[IN] The key to remove.
 * @return @c TM_TRUE if the key was found, @c TM_FALSE otherwise.
 */
TMH_DEF tm_bool TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_REMOVE)(TMH_X_NAME* map, const TMH_X_KEY key);

/*!
 * @brief Removes a key/value pair by index from the hashmap.
 * @param map[IN,OUT] The hashmap. Must not be @c NULL .
 * @param index[IN] The index into the key/value array.
 * @return @c TM_TRUE if index points to a valid entry in the hashmap, @c TM_FALSE otherwise.
 */
TMH_DEF tm_bool TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_REMOVE_INDEX)(TMH_X_NAME* map, tm_size_t index);

/*!
 * @brief Returns the count of inserted key/value pairs in the hashmap.
 * Note: Don't use this value for iteration, use map->capacity instead.
 * @param map[IN] The hashmap. Must not be @c NULL .
 * @return The count of inserted key/value pairs.
 */
TMH_DEF tm_size_t TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_COUNT)(const TMH_X_NAME* map);

/*!
 * @brief Returns the capacity for key/value pairs in the hashmap. Use this value for iteration.
 * @param map[IN] The hashmap. Must not be @c NULL .
 * @return The capacity for key/value pairs in the hashmap.
 */
TMH_DEF tm_size_t TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_CAPACITY)(const TMH_X_NAME* map);

/*!
 * @brief Returns whether a given index points to a tombstone entry.
 * @param map[IN] The hashmap. Must not be @c NULL .
 * @return @c TM_TRUE if the index was a tombstone entry, @c TM_FALSE otherwise.
 */
TMH_DEF tm_bool TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_IS_TOMBSTONE)(const TMH_X_NAME* map, tm_size_t index);

#ifdef TMH_INTERNAL_TESTING
extern tm_size_t TMH_STR_JOIN(TMH_X_FUNCPREFIX, _probe_length);
#endif

#ifdef __cplusplus
}
#endif

#endif /* defined(TMH_X_NAME) */

#ifdef TM_HASHMAPX_IMPLEMENTATION

#ifndef TM_ASSERT_VALID_SIZE
    #if defined(TM_SIZE_T_IS_SIGNED) && TM_SIZE_T_IS_SIGNED
        #define TM_ASSERT_VALID_SIZE(x) TM_ASSERT((x) >= 0)
    #else
        /* always true if size_t is unsigned */
        #define TM_ASSERT_VALID_SIZE(x) ((void)0)
    #endif
#endif /* !defined(TM_ASSERT_VALID_SIZE) */

#ifdef __cplusplus
extern "C" {
#endif

// Using fnv1a hashing algorithm.
// Note that the fnv1a algorithm is in the public domain.

#define TMH_FNV_64_PRIME 0x00000100000001B3ull
#define TMH_FNV_64_INIT 0xCBF29CE484222325ull

TMH_DEF uint64_t tmh_hash(const void* data, tm_size_t size) { return tmh_hash_combine(data, size, TMH_FNV_64_INIT); }
TMH_DEF uint64_t tmh_hash_combine(const void* data, tm_size_t size, uint64_t prev_hash) {
    uint64_t hval = prev_hash;

    if (size == 0) return prev_hash;

    const char* first = (const char*)data;
    const char* last = first + size;

    for (; first < last; ++first) {
        hval ^= (uint64_t)((unsigned char)*first);

#if defined(TMH_NO_FNV_GCC_OPTIMIZATION)
        hval *= TMH_FNV_64_PRIME;
#else
        hval += (hval << 1) + (hval << 4) + (hval << 5) + (hval << 7) + (hval << 8) + (hval << 40);
#endif
    }

    return hval;
}

TMH_DEF uint64_t tmh_string_hash(const char* str) { return tmh_string_hash_combine(str, TMH_FNV_64_INIT); }
TMH_DEF uint64_t tmh_string_hash_combine(const char* str, uint64_t prev_hash) {
    TM_ASSERT(str);
    uint64_t hval = prev_hash;

    for (const unsigned char* p = (const unsigned char*)str; *p; ++p) {
        hval ^= (uint64_t)*p;

#if defined(NO_FNV_GCC_OPTIMIZATION)
        hval *= FNV_64_PRIME;
#else
        hval += (hval << 1) + (hval << 4) + (hval << 5) + (hval << 7) + (hval << 8) + (hval << 40);
#endif
    }

    return hval;
}
TMH_DEF tm_bool tmh_is_power_of_two(uint64_t x) { return ((x) & (x - 1)) == 0; }

TMH_DEF tm_bool tmh_string_copy(char** out, const char* src) {
    TM_ASSERT(out);
    if (src) {
        size_t src_len = TM_STRLEN(src) + 1; // Including nullterminator.
        *out = (char*)TM_MALLOC(src_len * sizeof(char), sizeof(char));
        if (!*out) return TM_FALSE;
        TM_MEMCPY(*out, src, src_len * sizeof(char)); // Copy including nullterminator.
    }
    return TM_TRUE;
}
TMH_DEF tm_bool tmh_string_equals(const char* lhs, const char* rhs) {
    return (lhs == rhs) || (lhs && rhs && TM_STRCMP(lhs, rhs) == 0);
}
TMH_DEF void tmh_string_destroy(char** str) {
    if (str && *str) {
        TM_FREE(*str, (TM_STRLEN(*str) + 1) * sizeof(char), sizeof(char));
        *str = TM_NULL;
    }
}
TMH_DEF void tmh_string_destructive_move(char** dest, char** src) {
    TM_ASSERT(dest);
    TM_ASSERT(!*dest);
    TM_ASSERT(src);

    *dest = *src;
    *src = TM_NULL;
}

#ifdef __cplusplus
}
#endif

#endif /* defined(TM_HASHMAPX_IMPLEMENTATION) */

#ifdef TMH_X_IMPLEMENT

#ifdef __cplusplus
extern "C" {
#endif

#ifdef TMH_INTERNAL_TESTING
tm_size_t TMH_STR_JOIN(TMH_X_FUNCPREFIX, _probe_length) = 0;
#endif

TMH_DEF TMH_X_NAME TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_CREATE)() {
    TMH_X_NAME result;
    tm_size_t initial_capacity = 16;
    result.keys = TM_NULL;
    result.values = TM_NULL;
    result.count = 0;
    result.capacity = 0;
    result.keys = (TMH_X_KEY*)TM_MALLOC(initial_capacity * sizeof(TMH_X_KEY), sizeof(void*));
    if (result.keys) {
        result.values = (TMH_X_VALUE*)TM_MALLOC(initial_capacity * sizeof(TMH_X_VALUE), sizeof(void*));
        if (result.values) {
            result.capacity = initial_capacity;
        } else {
            TM_FREE(result.keys, initial_capacity * sizeof(TMH_X_KEY), sizeof(void*));
        }
    }
    for (tm_size_t i = 0, count = result.capacity; i < count; ++i) {
        TMH_X_KEY_INIT(result.keys[i]);
        TMH_X_KEY_SET_TOMBSTONE(result.keys[i]);
    }
    return result;
}

TMH_DEF void TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_DESTROY)(TMH_X_NAME* map) {
    if (!map) return;
    if (map->keys && map->values && map->count > 0) {
        for (tm_size_t i = 0, count = map->capacity; i < count; ++i) {
            if (TMH_X_KEY_IS_TOMBSTONE(map->keys[i])) {
                TMH_X_KEY_DESTROY(map->keys[i]);
                continue;
            }
            TMH_X_KEY_DESTROY(map->keys[i]);
            TMH_X_VALUE_DESTROY(map->values[i]);
        }
    }
    if (map->keys) {
        TM_FREE(map->keys, map->capacity * sizeof(TMH_X_KEY), sizeof(void*));
        map->keys = TM_NULL;
    }
    if (map->values) {
        TM_FREE(map->values, map->capacity * sizeof(TMH_X_VALUE), sizeof(void*));
        map->values = TM_NULL;
    }
    map->count = 0;
    map->capacity = 0;
}

TMH_DEF tm_bool TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_REHASH)(TMH_X_NAME* old_map, TMH_X_NAME* new_map) {
    for (tm_size_t i = 0, count = old_map->capacity; i < count; ++i) {
        if (TMH_X_KEY_IS_TOMBSTONE(old_map->keys[i])) continue;
        if (!TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_INSERT_MOVE)(new_map, &old_map->keys[i], &old_map->values[i]))
            return TM_FALSE;
        TMH_X_KEY_SET_TOMBSTONE(old_map->keys[i]);
        TMH_X_VALUE_DESTROY(old_map->values[i]);
    }
    return TM_TRUE;
}

TMH_DEF tm_bool TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_GROW)(TMH_X_NAME* map) {
    TMH_X_NAME new_map;
    new_map.capacity = map->capacity * 2;
    new_map.count = 0;
    new_map.keys = (TMH_X_KEY*)TM_MALLOC(new_map.capacity * sizeof(TMH_X_KEY), sizeof(void*));
    new_map.values = (TMH_X_VALUE*)TM_MALLOC(new_map.capacity * sizeof(TMH_X_VALUE), sizeof(void*));

    if (!new_map.keys || !new_map.values) {
        TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_DESTROY)(&new_map);
        return TM_FALSE;
    }
    for (tm_size_t i = 0, count = new_map.capacity; i < count; ++i) {
        TMH_X_KEY_INIT(new_map.keys[i]);
        TMH_X_KEY_SET_TOMBSTONE(new_map.keys[i]);
    }
    if (!TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_REHASH)(map, &new_map)) {
        TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_DESTROY)(&new_map);
        return TM_FALSE;
    }
    TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_DESTROY)(map);
    *map = new_map;
    return TM_TRUE;
}

TMH_DEF tm_bool TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_INSERT)(TMH_X_NAME* map, const TMH_X_KEY key,
                                                           const TMH_X_VALUE value) {
    if (TMH_X_KEY_IS_TOMBSTONE(key)) return TM_FALSE;

    TMH_X_KEY key_copy;
    if (!TMH_X_KEY_COPY(key_copy, key)) return TM_FALSE;
    TMH_X_VALUE value_copy;
    if (!TMH_X_VALUE_COPY(value_copy, value)) {
        // This here is the reason why C++ Classes are not supported.
        // It would manually call the destructor of a value on the stack!
        TMH_X_KEY_DESTROY(key_copy);
        return TM_FALSE;
    }

    if (!TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_INSERT_MOVE)(map, &key_copy, &value_copy)) {
        TMH_X_KEY_DESTROY(key_copy);
        TMH_X_VALUE_DESTROY(value_copy);
        return TM_FALSE;
    }
    return TM_TRUE;
}

TMH_DEF tm_bool TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_INSERT_MOVE)(TMH_X_NAME* map, TMH_X_KEY* key, TMH_X_VALUE* value) {
    TM_ASSERT(map);
    TM_ASSERT(map->keys);
    TM_ASSERT(map->values);
    TM_ASSERT(map->capacity > 0);
    TM_ASSERT(tmh_is_power_of_two((uint64_t)map->capacity));
    TM_ASSERT(map->count <= map->capacity);

    if (TMH_X_KEY_IS_TOMBSTONE(*key)) return TM_FALSE;

    // Check if map gets too full after addition.
    {
        uint64_t half = (uint64_t)(map->capacity) / 2;
        uint64_t quarter = half / 2;
        if (map->count + 1 >= half + quarter) {
            if (!TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_GROW)(map)) return TM_FALSE;
        }
    }

    for (;;) {
        uint64_t mask = (uint64_t)(map->capacity - 1);
        uint64_t hash = TMH_X_KEY_HASH(*key);
        uint64_t index = hash & mask;
#ifdef TMH_INTERNAL_TESTING
        printf("Key %s: hash: %llu; index: %llu\n", *key, hash, index);
#endif
        TM_ASSERT(index < (uint64_t)map->capacity);

        tm_bool slot_found = TM_FALSE;
        tm_bool slot_existing = TM_FALSE;
        tm_size_t slot_index = 0;

        for (tm_size_t i = 0; i < TMH_MAX_LINEAR_PROBE; ++i) {
            uint64_t new_index = (index + i) & mask;
            if (!slot_found && (TMH_X_KEY_IS_TOMBSTONE(map->keys[new_index]))) {
                slot_index = new_index;
                slot_found = TM_TRUE;
                continue;
            }
            if (TMH_X_KEY_EQUALS(map->keys[new_index], *key)) {
                slot_index = new_index;
                slot_found = TM_TRUE;
                slot_existing = TM_TRUE;
                break;
            } else {
#ifdef TMH_INTERNAL_TESTING
                if (i == 0 && !TMH_X_KEY_IS_TOMBSTONE(map->keys[new_index])) {
                    printf("Conflict found between %s and %s\n", *key, map->keys[new_index]);
                }
#endif
            }
        }
        if (slot_found) {
            TMH_X_KEY_DESTROY(map->keys[slot_index]);
            TMH_X_KEY_MOVE(map->keys[slot_index], key);

            if (slot_existing) {
                TMH_X_VALUE_DESTROY(map->values[slot_index]);
            }
            TMH_X_VALUE_MOVE(map->values[slot_index], value);

            if (!slot_existing) ++map->count;
            return TM_TRUE;
        }

        if (!TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_GROW)(map)) return TM_FALSE;
    }
}

TMH_DEF tm_bool TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_FIND_INDEX)(const TMH_X_NAME* map, const TMH_X_KEY key,
                                                               tm_size_t* index_out) {
    TM_ASSERT(map);
    TM_ASSERT(map->keys);
    TM_ASSERT(map->values);
    TM_ASSERT(map->capacity > 0);
    TM_ASSERT(tmh_is_power_of_two((uint64_t)map->capacity));
    TM_ASSERT(map->count <= map->capacity);
    TM_ASSERT(index_out);

    if (map->count == 0) return TM_FALSE;

#ifdef TMH_INTERNAL_TESTING
    TMH_STR_JOIN(TMH_X_FUNCPREFIX, _probe_length) = 0;
#endif

    uint64_t mask = (uint64_t)(map->capacity - 1);
    uint64_t index = TMH_X_KEY_HASH(key) & mask;
    TM_ASSERT(index < (uint64_t)map->capacity);

    for (tm_size_t i = 0; i < TMH_MAX_LINEAR_PROBE; ++i) {
        uint64_t new_index = (index + i) & mask;
        if (TMH_X_KEY_EQUALS(map->keys[new_index], key)) {
            *index_out = (tm_size_t)new_index;
            return TM_TRUE;
        }
#ifdef TMH_INTERNAL_TESTING
        TMH_STR_JOIN(TMH_X_FUNCPREFIX, _probe_length) += 1;
#endif
    }
    return TM_FALSE;
}

TMH_DEF TMH_X_VALUE* TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_FIND)(const TMH_X_NAME* map, const TMH_X_KEY key) {
    tm_size_t index = 0;
    if (TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_FIND_INDEX)(map, key, &index)) {
        return &map->values[index];
    }
    return TM_NULL;
}

TMH_DEF TMH_STR_JOIN(TMH_X_NAME, TMH_PAIR)
    TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_GET)(const TMH_X_NAME* map, tm_size_t index) {
    TM_ASSERT(map);
    TM_ASSERT(map->keys);
    TM_ASSERT(map->values);
    TM_ASSERT(map->capacity > 0);
    TM_ASSERT(tmh_is_power_of_two((uint64_t)map->capacity));
    TM_ASSERT(map->count <= map->capacity);
    TM_ASSERT_VALID_SIZE(index);
    TMH_STR_JOIN(TMH_X_NAME, TMH_PAIR) result;
    result.key = TM_NULL;
    result.value = TM_NULL;
    if (index < map->capacity && !TMH_X_KEY_IS_TOMBSTONE(map->keys[index])) {
        result.key = &map->keys[index];
        result.value = &map->values[index];
    }
    return result;
}

TMH_DEF tm_bool TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_REMOVE)(TMH_X_NAME* map, const TMH_X_KEY key) {
    TM_ASSERT(map);

    tm_size_t index = 0;
    if (!TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_FIND_INDEX)(map, key, &index)) return TM_FALSE;
    --map->count;
    TMH_X_KEY_DESTROY(map->keys[index]);
    TMH_X_VALUE_DESTROY(map->values[index]);
    TMH_X_KEY_INIT(map->keys[index]);
    TMH_X_KEY_SET_TOMBSTONE(map->keys[index]);
    return TM_TRUE;
}

TMH_DEF tm_bool TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_REMOVE_INDEX)(TMH_X_NAME* map, tm_size_t index) {
    TM_ASSERT(map);
    TM_ASSERT(map->count > 0);

    TM_ASSERT_VALID_SIZE(index);
    if (index < map->capacity && !TMH_X_KEY_IS_TOMBSTONE(map->keys[index])) {
        --map->count;
        TMH_X_KEY_DESTROY(map->keys[index]);
        TMH_X_VALUE_DESTROY(map->values[index]);
        TMH_X_KEY_INIT(map->keys[index]);
        TMH_X_KEY_SET_TOMBSTONE(map->keys[index]);
        return TM_TRUE;
    }
    return TM_FALSE;
}

TMH_DEF tm_size_t TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_COUNT)(const TMH_X_NAME* map) {
    TM_ASSERT(map);
    return map->count;
}
TMH_DEF tm_size_t TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_CAPACITY)(const TMH_X_NAME* map) {
    TM_ASSERT(map);
    return map->capacity;
}
TMH_DEF tm_bool TMH_STR_JOIN(TMH_X_FUNCPREFIX, TMH_IS_TOMBSTONE)(const TMH_X_NAME* map, tm_size_t index) {
    TM_ASSERT(map);
    TM_ASSERT_VALID_SIZE(index);
    TM_ASSERT(index < map->capacity);
    return TMH_X_KEY_IS_TOMBSTONE(map->keys[index]);
}

#ifdef __cplusplus
}
#endif

#endif /* defined(TMH_X_IMPLEMENT) */

// clang-format off
#ifdef TMH_X_NAME
    #undef TMH_X_NAME
    #undef TMH_X_FUNCPREFIX
    #undef TMH_X_KEY
    #undef TMH_X_KEY_INIT
    #undef TMH_X_KEY_EQUALS
    #undef TMH_X_KEY_COPY
    #undef TMH_X_KEY_MOVE
    #undef TMH_X_KEY_DESTROY
    #undef TMH_X_KEY_HASH
    #undef TMH_X_KEY_IS_TOMBSTONE
    #undef TMH_X_KEY_SET_TOMBSTONE
    #undef TMH_X_VALUE
    #undef TMH_X_VALUE_COPY
    #undef TMH_X_VALUE_INIT
    #undef TMH_X_VALUE_DESTROY
    #undef TMH_X_VALUE_MOVE
    #ifdef TMH_X_VALUE_IS_TRIVIAL
        #undef TMH_X_VALUE_IS_TRIVIAL
    #endif
    #ifdef TMH_X_KEY_IS_STRING
        #undef TMH_X_KEY_IS_STRING
    #endif
#endif
// clang-format on

/*
There are two licenses you can freely choose from - MIT or Public Domain
---------------------------------------------------------------------------

MIT License:
Copyright (c) 2020 Tolga Mizrak

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---------------------------------------------------------------------------

Public Domain (www.unlicense.org):
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>

---------------------------------------------------------------------------
*/